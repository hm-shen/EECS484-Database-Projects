-- Part 3 Database loading

-- Sequence for auto_increment 
create sequence seq_cities
start with 1
increment by 1;

create sequence seq_prog
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

-- Auto_increment Trigger for prog
create or replace trigger prog_trigger
before insert on programs
for each row
begin
  :new.program_id := seq_prog.nextval;
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

insert into programs (institution, concentration, degree)
select distinct institution_name, program_concentration, program_degree from weile.public_user_information; 

drop sequence seq_prog;

insert into user_events (event_id, event_creator_id, event_name, event_tagline, event_description, event_host, event_type, event_subtype, event_location, event_start_time, event_end_time) 
select distinct event_id, event_creator_id, event_name, event_tagline, event_description, event_host, event_type, event_subtype, event_location, event_start_time, event_end_time from weile, public_user_information;




