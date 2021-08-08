

/* user info table */
CREATE TABLE cws_user.user_info (
    `user_uid` varchar(32) NOT NULL COMMENT 'internal uid',
    `oid` varchar(32) NOT NULL COMMENT 'id which shows',
    `user_name` varchar(64) NOT NULL,
    `phone_number` varchar(32) NOT NULL DEFAULT '',
    `gender` tinyint NOT NULL DEFAULT '0',
    `birthday` datetime NOT NULL DEFAULT '1970-01-01 00:00:00',
    `password` varchar(64) NOT NULL DEFAULT '',
    `create_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00',
    PRIMARY KEY (`user_uid`),
    UNIQUE KEY `oid` (`oid`),
    KEY `user_name` (`user_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT 'user info table';
