#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
import argparse
import hashlib
import os
import subprocess
import sys

def parse_args():
    parser = argparse.ArgumentParser(description='Binary signing tool')
    parser.add_argument('-b', '--bin', help='Unsigned binary')
    parser.add_argument('-o', '--out', help='Output file')
    parser.add_argument('-p', '--publickey', help='Public key file');
    parser.add_argument('-s', '--privatekey', help='Private(secret) key file')
    return parser.parse_args()

def sign_and_write(data, priv_key, out_file):
    """Signs the data (bytes) with the private key (file path)."""
    """Save the signed firmware to out_file (file path)."""

    signcmd = [ 'openssl', 'dgst', '-sha256', '-sign', priv_key ]
    proc = subprocess.Popen(signcmd, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE)
    signout, signerr = proc.communicate(input=data)
    if proc.returncode:
        sys.stderr.write("OpenSSL returned an error signing the binary: " + str(proc.returncode) + "\nSTDERR: " + str(signerr))
    else:
        print("Length of signout: " + str(len(signout)))
        with open(out_file, "wb") as out:
            out.write(signout)
            out.write(data)
            sys.stderr.write("Signed binary: " + out_file + "\n")

def main():
    args = parse_args()
    if not os.path.isfile(args.privatekey):
        return
    try:
        with open(args.bin, "rb") as b:
            bin = b.read()

            sign_and_write(bin, args.privatekey, args.out)

    except Exception as e:
        sys.stderr.write(str(e))
        sys.stderr.write("Not signing the generated binary\n")
    return 0

if __name__ == '__main__':
    sys.exit(main())

