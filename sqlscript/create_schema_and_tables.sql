create database if not exists x_login;

create table if not exists x_login.user (
    `id` bigint(64) NOT NULL,
    `act` varchar(255) NOT NULL,
    `pwd` varchar(255) NULL,
    PRIMARY KEY (`id`)
);

create table if not exists x_login.serverlist (
    `id` int(8) NOT NULL,
	`name` varchar(16) NOT NULL DEFAULT 'UNDEFINED',
    `ip` varchar(16) NOT NULL DEFAULT '127.0.0.1',
    `port` int(11) NOT NULL DEFAULT '0',
	`state` int(4) NOT NULL DEFAULT '0',
    PRIMARY KEY (`id`)
);