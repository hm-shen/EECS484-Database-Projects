--need info: name of state, number of event

create view most_event as 
select UE.event_city_id,count(UE.event_id) as cont
from user_events UE
group by UE.event_city_id
order by cont desc;

select C.state_name ,ME.cont
from most_event ME, cities C
where C.city_id = ME.event_city_id
and ME.cont = 
(select max(ME1.cont)
 from most_event ME1
);

drop view most_event;
--how to write without view?