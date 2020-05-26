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

create database if not exists x_data;

create table if not exists x_data.role (
    `actid` bigint(64) NOT NULL,
    `roleid` bigint(64) NOT NULL,
    `name` varchar(255) NOT NULL,
    PRIMARY KEY (`actid`,`roleid`)
);

create table if not exists x_data.global (
    `var` varchar(32) NOT NULL,
    `value` int(64) NOT NULL DEFAULT '0',
    PRIMARY KEY (`var`)
);