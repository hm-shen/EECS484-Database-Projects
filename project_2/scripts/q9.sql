-- need info: id1,firstname1,lastname1
--            id2,firstname2,lastname2

select U1.user_id,U1.first_name,U1.last_name,
U2.user_id,U2.first_name,U2.last_name
from users U1, users U2, friends F,user_hometown_city UH1, user_hometown_city UH2
where U1.user_id < U2.user_id and U1.last_name = U2.last_name
and (   U1.user_id = F.user1_id and U2.user_id = F.user2_id
     or U1.user_id = F.user2_id and U2.user_id = F.user1_id)
and U1.user_id = UH1.user_id and U2.user_id = UH2.user_id and UH1.hometown_city_id = UH2.hometown_city_id
and U1.year_of_birth is not null and U2.year_of_birth is not null
and (U1.year_of_birth < (U2.year_of_birth + 10) or U2.year_of_birth < (U1.year_of_birth + 10))
order by U1.user_id asc , U2.user_id asc; 
