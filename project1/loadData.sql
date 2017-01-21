-- Part 3 Database loading

-- Sequence for auto_increment 
create sequence seq_cities
start with 1
increment by 1;

-- Auto_increment Trigger for city 
create or replace trigger city_trigger
before insert on cities
for each row
begin
  :new.city_id := seq_cities.nextval;
--  select seq_data.nextval into :new.city_id from dual;
end;
/

insert into users (user_id, first_name, last_name, year_of_birth, month_of_birth, day_of_birth, gender) 
select distinct user_id, first_name, last_name, year_of_birth, month_of_birth, day_of_birth, gender from weile.public_user_information;

insert into friends (user1_id, user2_id)
select distinct user1_id, user2_id from weile.public_are_friends;

insert into cities (city_name, state_name, country_name)
select distinct hometown_city, hometown_state, hometown_country from weile.public_user_information 
union
select distinct current_city, current_state, current_country from weile.public_user_information
union
select distinct event_city, event_state, event_country from weile.public_event_information;


drop sequence seq_cities;

