create database if not exists app;
use app;
-- MariaDB dump 10.19  Distrib 10.5.10-MariaDB, for Linux (x86_64)
--
-- Host: 192.168.0.107    Database: app
-- ------------------------------------------------------
-- Server version	8.0.25

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `bestellungen`
--

DROP TABLE IF EXISTS `bestellungen`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bestellungen` (
  `id` int NOT NULL AUTO_INCREMENT,
  `initialgewicht` double NOT NULL,
  `zeitpunkt_aufgetrunken` datetime DEFAULT NULL,
  `zeitpunkt_bestellt` datetime DEFAULT NULL,
  `zeitpunkt_geliefert` datetime DEFAULT NULL,
  `zeitpunkt_vorbereitet` datetime DEFAULT NULL,
  `getraenk_id` int DEFAULT NULL,
  `glas_id` int DEFAULT NULL,
  `kunde_id` int DEFAULT NULL,
  `platz_id` int DEFAULT NULL,
  `status_id` int DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `FKl5s1xdxwlh4jf1scetoefq37l` (`getraenk_id`),
  KEY `FKqmk82tnhwq8ls9w5xwy7ysl4y` (`glas_id`),
  KEY `FK1tblep6o4gnx8uwsucc4i1ho` (`kunde_id`),
  KEY `FK48sjm44ger7ofdrj5p4j0o34s` (`platz_id`),
  KEY `FKjw6brva7lsieeg7ptj58w16jm` (`status_id`)
) ENGINE=MyISAM AUTO_INCREMENT=1 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `getraenke`
--

DROP TABLE IF EXISTS `getraenke`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `getraenke` (
  `id` int NOT NULL AUTO_INCREMENT,
  `alkoholgehalt` double NOT NULL,
  `groesse` int NOT NULL,
  `name` varchar(255) DEFAULT NULL,
  `preis` double NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=1 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `glaeser`
--

DROP TABLE IF EXISTS `glaeser`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `glaeser` (
  `id` int NOT NULL AUTO_INCREMENT,
  `leergewicht` int NOT NULL,
  `name` varchar(255) DEFAULT NULL,
  `rfid` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=1 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `kunden`
--

DROP TABLE IF EXISTS `kunden`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `kunden` (
  `id` int NOT NULL AUTO_INCREMENT,
  `bezahlt` tinyint(1) DEFAULT '0',
  `name` varchar(255) DEFAULT NULL,
  `zeitpunkt_abgeschlossen` datetime DEFAULT NULL,
  `zeitpunkt_angelegt` datetime DEFAULT NULL,
  `platz_id` int DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `FKgdohwbqp7vrxmd6amdu9rcgxp` (`platz_id`)
) ENGINE=MyISAM AUTO_INCREMENT=1 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `messpunkte`
--

DROP TABLE IF EXISTS `messpunkte`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `messpunkte` (
  `id` int NOT NULL AUTO_INCREMENT,
  `fuellstand` double NOT NULL,
  `zeitpunkt` datetime DEFAULT NULL,
  `bestellungen_id` int DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `FKf5ybvt22dh6ovww6of2mpcj19` (`bestellungen_id`)
) ENGINE=MyISAM AUTO_INCREMENT=1 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `plaetze`
--

DROP TABLE IF EXISTS `plaetze`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `plaetze` (
  `id` int NOT NULL AUTO_INCREMENT,
  `name` varchar(255) DEFAULT NULL,
  `tisch_id` int DEFAULT NULL,
  `mac` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `FKfaxjgcxd1r6lhd3tayk2kqqab` (`tisch_id`)
) ENGINE=MyISAM AUTO_INCREMENT=1 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `stati`
--

DROP TABLE IF EXISTS `stati`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `stati` (
  `id` int NOT NULL AUTO_INCREMENT,
  `bezeichnung` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=1 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `test_table`
--

DROP TABLE IF EXISTS `test_table`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `test_table` (
  `id` int NOT NULL AUTO_INCREMENT,
  `name` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=1 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `tische`
--

DROP TABLE IF EXISTS `tische`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tische` (
  `id` int NOT NULL AUTO_INCREMENT,
  `name` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=1 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2021-06-28 15:49:57
