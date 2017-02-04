-- need information: id ,firstname , last name 
select U.user_id, U.first_name, U.last_name 
from users U
where not exists
(select F1.user1_id
 from friends F1 
 where F1.user1_id = U.user_id)
and 
not exists
(select F1.user2_id
from friends F1
where F1.user2_id = U.user_id)
order by U.user_id ASC;