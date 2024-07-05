BEGIN TRANSACTION;
insert into `sample_model`(`id`,`count`,`description`) values(1, 100, 'Sample 1');
insert into `sample_model`(`id`,`count`,`description`) values(2, 200, 'Sample 2');
COMMIT;