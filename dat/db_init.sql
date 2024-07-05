BEGIN TRANSACTION;

DROP TABLE IF EXISTS `sample_model`;
CREATE TABLE IF NOT EXISTS `sample_model` (
	`id`	INTEGER NOT NULL,
	`count`	INTEGER NOT NULL,
	`description`	TEXT NOT NULL,
	PRIMARY KEY(`id`)
);

CREATE INDEX `sample_model_count_idx` ON `sample_model` ( `count` );

COMMIT;