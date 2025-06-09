INSERT INTO "API" ("Host", "Name")
VALUES ('https://www.virustotal.com/api/v3/', 'Virustotal');

INSERT INTO "API_key" ("key", "API_ID")
VALUES 
('ce81af1dcbe39882a498192121a1f3de2c21a43a347a63651ac76032db5f2b99', 1), 
('549c0aef9bfa08df7faee7f590a43bba9d2f5ec4843351554e84f3f8deff6ab3', 1);

INSERT INTO "API_endpoint" ("endpoint", "name", "API_ID")
VALUES
('domains/', 'Domain', 1),
('urls/', 'Url', 1),
('ip_addresses/', 'Ip', 1);
