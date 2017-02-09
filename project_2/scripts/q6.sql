SET TIMING ON;

CREATE OR REPLACE VIEW common_friends AS
SELECT temp.user1_id AS user1_id, temp.user2_id AS user2_id, COUNT(*) AS cont
FROM 
( 
SELECT F1.user1_id AS user1_id, F2.user2_id AS user2_id 
FROM syzhao.public_friends F1, syzhao.public_friends F2
WHERE F1.user2_id = F2.user1_id  
AND (F1.user1_id, F2.user2_id) NOT IN (SELECT DISTINCT * FROM syzhao.public_friends F3)
UNION ALL
SELECT F1.user1_id AS user1_id, F2.user1_id AS user2_id 
FROM syzhao.public_friends F1, syzhao.public_friends F2
WHERE F1.user2_id = F2.user2_id AND F1.user1_id < F2.user1_id
AND (F1.user1_id, F2.user1_id) NOT IN (SELECT DISTINCT * FROM syzhao.public_friends F3)
UNION ALL
SELECT F1.user2_id AS user1_id, F2.user2_id AS user2_id 
FROM syzhao.public_friends F1, syzhao.public_friends F2
WHERE F1.user1_id = F2.user1_id AND F1.user2_id < F2.user2_id
AND (F1.user2_id, F2.user2_id) NOT IN (SELECT DISTINCT * FROM syzhao.public_friends F3)
) temp
GROUP BY temp.user1_id, temp.user2_id
ORDER BY COUNT(*) DESC, temp.user1_id ASC, temp.user2_id ASC;

SELECT CF.user1_id, CF.user2_id, U1.first_name, U1.last_name, U2.first_name, U2.last_name
FROM 
(SELECT comf.user1_id, comf.user2_id, comf.cont 
FROM common_friends comf 
WHERE ROWNUM <= 6 ) CF, 
syzhao.public_users U1, syzhao.public_users U2
WHERE U1.user_id = CF.user1_id AND U2.user_id = CF.user2_id
ORDER BY CF.cont DESC, CF.user1_id, CF.user2_id;

SELECT U.user_id, U.first_name, U.last_name
FROM syzhao.public_users U,
(
SELECT F1.user2_id AS user_id
FROM syzhao.public_friends F1, syzhao.public_friends F2
WHERE (F1.user1_id = 109 AND F2.user2_id = 122 AND F1.user2_id = F2.user1_id) 
UNION ALL
SELECT F1.user1_id AS user_id
FROM syzhao.public_friends F1, syzhao.public_friends F2
WHERE (F1.user2_id = 109 AND F2.user2_id = 122 AND F1.user1_id = F2.user1_id)
UNION ALL
SELECT F1.user2_id AS user_id
FROM syzhao.public_friends F1, syzhao.public_friends F2
WHERE (F1.user1_id = 109 AND F2.user1_id = 122 AND F1.user2_id = F2.user2_id)
) FRD
WHERE U.user_id = FRD.user_id
ORDER BY U.user_id;


DROP VIEW common_friends;
