-- need info: userid, firstname,lastname
--            photoId, albumId, albumName, photoCaption, photoLink

create view tag_info as
select T.tag_photo_id,count(T.tag_photo_id) as cont,
from tags T
group by T.tag_photo_id
order by cont DESC, T.tag_photo_id ASC;

select U.user_id, U.first_name, U.last_name,P.photo_id, P.album_id, A.album_name, P.photo_caption, P.photo_link
from users U, photos P, albums A, tag_info TI, tags T
where TI.tag_photo_id = T.tag_photo_id 
and T.tag_photo_id = P.photo_id and P.album_id = A.album_id and T.tag_subject_id = U.user_id 
and TI.cont = 
(select distinct T1.cont
 from tag_info T1
 where rownum = 1)
order by TI.cont DESC, T.tag_photo_id ASC;  

drop view tag_info;


--for test:
select * from tag_info;

select count(T.tag_photo_id)
from tag_info T
where T.cont = 4;