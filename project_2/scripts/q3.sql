-- need information: id ,firstname, lastname

select U.user_id, U.first_name, U.last_name,
from users U ,user_current_city UC, user_hometown_city UH
where (U.user_id = UC.user_id and U.user_id = UH.user_id and UC.current_city_id <> UH.hometown_city_id)
or not exists
(select U1.user_id 
 from users U1, user_current_city UC1
 where UC1.user_id = U1.user_id
)
or not exists
(select U2.user_id 
 from users U2, user_hometown_city UH1
 where UH1.user_id = U2.user_id
)
order by U.user_id;
