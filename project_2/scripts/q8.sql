
--在create view里面把年月日排好顺序还是在select里面做这些事？
--在select语句直接排序然后选择rownum还是先选year再排序？

create view his_friend as
select U.user_id as whos_friend_id, U1.user_id as friends_id
from users U, users U1, friends F
where U.user_id = 1 and ((U.user_id = F.user1_id and U1.user_id = F.user2_id)
or (U.user_id = F.user2_id and U1.user_id = F.user1_id));
--chage user_id above!~

-- for max_age:
select U.user_id, U.first_name, U.last_name
from users U , his_friend hf 
where U.year_of_birth = 
(select min(U2.year_of_birth) 
 	from users U2, his_friend hf1
 	where U2.year_of_birth is not null
 	and U2.user_id = hf1.friends_id
)
and U.user_id = hf.friends_id
and rownum = 1
order by U.month_of_birth ASC, U.day_of_birth ASC, U.user_id DESC;

-- for min_age:
select U.user_id, U.first_name, U.last_name
from users U , his_friend hf 
where U.year_of_birth = 
(select max(U2.year_of_birth) 
 	from users U2, his_friend hf1
 	where U2.year_of_birth is not null
 	and U2.user_id = hf1.friends_id
)
and U.user_id = hf.friends_id
and rownum = 1
order by U.month_of_birth DESC, U.day_of_birth DESC, U.user_id ASC;

drop view his_friend;


---------------------------for test-----------------------------------
select min(U3.year_of_birth) 
from users U3
where U3.year_of_birth is not null;

select U3.user_id,U3.year_of_birth
from users U3
where U3.year_of_birth <100;
