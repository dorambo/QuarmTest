ALTER TABLE `raid_members` 
ADD COLUMN `guildid` int(11) UNSIGNED NOT NULL DEFAULT 0 AFTER `islooter`,
ADD COLUMN `is_officer` tinyint(1) UNSIGNED NOT NULL DEFAULT 0 AFTER `guildid`;

ALTER TABLE `spawn2` 
ADD COLUMN `rotation_id` int(11) NOT NULL DEFAULT 0 AFTER `raid_target_spawnpoint`;