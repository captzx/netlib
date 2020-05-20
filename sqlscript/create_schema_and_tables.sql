create database if not exists x_login;

create table if not exists x_login.user (
    `id` bigint(64) NOT NULL,
    `act` varchar(255) NOT NULL,
    `pwd` varchar(255) NULL,
    PRIMARY KEY (`id`));