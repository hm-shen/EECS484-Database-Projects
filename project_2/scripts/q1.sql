--maximum length
--version 1:
select max(length(first_name))
from users;

select distinct U.first_name 
from users U
where length(U.first_name) = 9
order by U.first_name ASC;

-- version 2
select distinct U.first_name 
from users U
where length(U.first_name) = 
(select max(length(U1.first_name))
from users U1)
order by U.first_name ASC;

-----------------------------------------
--minimum length
--version 1
select min(length(first_name))
from users;

select distinct U.first_name 
from users U
where length(U.first_name) = 4
order by U.first_name ASC;

-- version 2
select distinct U.first_name 
from users U
where length(U.first_name) = 
(select min(length(U1.first_name))
from users U1)
order by U.first_name ASC;

----------------------------------------
--most common name
create view common_name as 
select distinct U.first_name, count(U.first_name) as cont
From users U
group by U.first_name     -- without group by it will return an error 
order by count(U.first_name) DESC;   

select C.first_name
from common_name C
where C.cont = 
(select max(C1.cont) 
 from common_name C1)
order by U.first_name ASC;

drop view common_name;

-- for test:
select max(user_id)
from users;

insert into users values(800, 'abcdefghi', 'nevermind', 1994, 12, 5, 'female') ;
