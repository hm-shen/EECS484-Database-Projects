-- syzhao.public_
-- need info: a pair of userid,userfirstname,userlastname, year of birth,
--           shared photo id, albumid ,albumname, caption, photolink 
drop view share_photo;

create view share_photo as
select U1.user_id as u1id, U2.user_id as u2id ,count(*) as cont
from syzhao.public_users U1, syzhao.public_users U2, syzhao.public_tags T1,syzhao.public_tags T2
where U1.user_id = T1.tag_subject_id and U2.user_id = T2.tag_subject_id and T1.tag_photo_id = T2.tag_photo_id
and U1.user_id < U2.user_id and U1.gender = U2.gender 
and U1.year_of_birth is not null and U2.year_of_birth is not null and abs(U1.year_of_birth - U2.year_of_birth)< = 20
group by U1.user_id,U2.user_id
order by cont DESC, u1id ASC, u2id ASC;

select * from share_photo;

select U1.user_id, U1.first_name, U1.last_name, U1.year_of_birth, U2.user_id, U2.first_name, U2.last_name, U2.year_of_birth,
       P.photo_id, P.album_id, A.album_name, P.photo_caption, P.photo_link
from syzhao.public_users U1, syzhao.public_users U2, share_photo s, syzhao.public_photos P, syzhao.public_albums A, syzhao.public_tags T1,syzhao.public_tags T2
where (U1.user_id, U2.user_id )in (select s.u1id, s.u2id from share_photo s)
and s.u1id = U1.user_id and s.u2id = U2.user_id
and U1.user_id = T1.tag_subject_id and U2.user_id = T2.tag_subject_id and T1.tag_photo_id = T2.tag_photo_id
and not exists( select F.user1_id ,F.user2_id from syzhao.public_friends F where U1.user_id = F.user1_id and U2.user_id = F.user2_id)
and T1.tag_photo_id = P.photo_id and P.album_id = A.album_id
and rownum <= 5 
order by s.cont DESC ,U1.user_id ASC,  U2.user_id ASC;

drop view share_photo;
--test if in order of cont???