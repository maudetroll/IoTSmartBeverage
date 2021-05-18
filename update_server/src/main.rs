#![feature(str_split_once)]
use actix_web::{
    get, http::HeaderMap, web, App, Error, HttpRequest, HttpResponse, HttpServer, Responder,
};
use base64;
use futures::{future::ok, stream::once};
use openssl::ssl::{SslAcceptor, SslFiletype, SslMethod};
mod secret;
use crate::secret::secret::SECRET;
use env_logger::{Builder, WriteStyle};
use log::LevelFilter;
use log::{debug, warn};
use md5;
use std::io::prelude::Read;

static CURRENT_VERSION: &str = "VERSION_0.0.0";
static CURRENT_PATH: &str = "srv/esp-builds/latest.ino.bin.signed";

async fn index(_req: HttpRequest) -> impl Responder {
    "Hello this site is self signed."
}

fn check_header(header_map: &HeaderMap, key: &str, check_value: &str) -> bool {
    if let Some(header_value) = header_map.get(key) {
        if header_value == check_value {
            return true;
        }
        warn!(
            "Received Header: {:?} with value: {:?} did not match expected value: {:?}",
            key, header_value, check_value
        );
    }
    false
}

#[get("/update")]
async fn update(req: HttpRequest) -> HttpResponse {
    let header_map = req.headers();

    if !check_header(header_map, "User-Agent", "ESP8266-http-Update") {
        return HttpResponse::Forbidden().body("Header do not conform to expected device");
    }

    if !header_map.contains_key("x-ESP8266-STA-MAC")
        || !header_map.contains_key("x-ESP8266-AP-MAC")
        || !header_map.contains_key("x-ESP8266-free-space")
        || !header_map.contains_key("x-ESP8266-sketch-size")
        || !header_map.contains_key("x-ESP8266-sketch-md5")
        || !header_map.contains_key("x-ESP8266-chip-size")
        || !header_map.contains_key("x-ESP8266-sdk-version")
    {
        warn!("Request with insufficient header keys received");
        return HttpResponse::Forbidden().body("Header do not conform to expected device");
    }
    debug!("Header were all right");
    // Should maybe create wrapper function to reduce this whole Option and Result mess
    if let Some(version_header) = header_map.get("x-ESP8266-version") {
        if let Ok(result_as_vec) = base64::decode(version_header) {
            if let Ok(result) = String::from_utf8(result_as_vec) {
                if let Some((mut version_part, mut secret_part)) = result.split_once("::") {
                    // Only check if version is the same, allows to backport ESPs
                    version_part = version_part.trim();
                    secret_part = secret_part.trim();
                    if version_part != CURRENT_VERSION && secret_part == SECRET {
                        if let Ok(mut file) = std::fs::File::open(CURRENT_PATH) {
                            // Load file and return
                            let mut buffer = Vec::<u8>::new();
                            if let Ok(_) = file.read_to_end(&mut buffer) {
                                if !check_header(
                                    header_map,
                                    "x-ESP8266-sketch-md5",
                                    &format!("{:?}", md5::compute(&buffer)),
                                ) {
                                    let body = once(ok::<_, Error>(web::Bytes::from(buffer)));
                                    return HttpResponse::Ok()
                                        .content_type("application/octet-stream")
                                        .streaming(body);
                                } else {
                                    debug!("MD5 of local and esp were the same");
                                }
                            } else {
                                debug!("File read failed");
                            }
                        } else {
                            debug!("File open failed");
                        }
                    } else {
                        debug!("Version and secret failed");
                    }
                } else {
                    debug!("Split failed");
                }
            } else {
                debug!("UTF8 conversion failed");
            }
        } else {
            debug!("Decode failed");
        }
    } else {
        debug!("Version header does not exist");
    }

    debug!("Request was not modified");
    HttpResponse::NotModified().finish()
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    let mut log_builder = Builder::new();
    log_builder
        .filter(None, LevelFilter::Info)
        .write_style(WriteStyle::Always)
        .init();
    // load ssl keys
    // to create a self-signed temporary cert for testing:
    // `openssl req -x509 -newkey rsa:4096 -nodes -keyout key.pem -out cert.pem -days 365 -subj '/CN=localhost'`
    let mut ssl_builder = SslAcceptor::mozilla_intermediate(SslMethod::tls()).unwrap();
    let path = String::from("./self-signed-cert/");
    ssl_builder
        .set_private_key_file(path.to_string().clone() + "key.pem", SslFiletype::PEM)
        .unwrap();
    ssl_builder
        .set_certificate_chain_file(path.to_string().clone() + "cert.pem")
        .unwrap();

    HttpServer::new(|| App::new().service(update).route("/", web::get().to(index)))
        .bind_openssl("127.0.0.1:8080", ssl_builder)?
        .run()
        .await
}
