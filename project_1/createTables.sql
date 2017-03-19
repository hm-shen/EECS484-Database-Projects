create table users (user_id number, 
                    first_name varchar2(100) not null, 
                    last_name varchar2(100) not null,
                    year_of_birth integer, 
                    month_of_birth integer, 
                    day_of_birth integer, 
                    gender varchar2(100),
                    primary key (user_id));

create table friends (user1_id number,
                      user2_id number,
                      primary key (user1_id, user2_id),
                      foreign key (user1_id) references users(user_id)
                      on delete cascade,
                      foreign key (user2_id) references users(user_id)
                      on delete cascade);

-- If A and B are friends with each other, and A quits fakebook because he believes it sucks, then A and B should no longer be defined as friends in fakebook.
-- It seems illegal to "change" your friend from C to D. Thus change a user_id is not allowed.

create table cities (city_id integer,
                     city_name varchar2(100),
                     state_name varchar2(100),
                     country_name varchar2(100),
                     primary key (city_id));

create table user_current_city (user_id number,
                                current_city_id integer not null,
                                primary key (user_id),
                                foreign key (user_id) references users (user_id)
                                on delete cascade,
                                foreign key (current_city_id) references cities(city_id)
                                on delete cascade);

-- 

create table user_hometown_city (user_id number,
                                 hometown_city_id integer not null,
                                 primary key (user_id),
                                 foreign key (user_id) references users (user_id)
                                 on delete cascade,
                                 foreign key (hometown_city_id) references cities(city_id)
                                 on delete cascade);
                                 --on update cascade);

create table message (message_id integer,
                      sender_id number not null,
                      receiver_id number not null,
                      message_content varchar2(2000),
                      sent_time timestamp not null,
                      primary key (message_id),
                      foreign key (sender_id) references users(user_id)
                      on delete cascade,
                      foreign key (receiver_id) references users(user_id)
                      on delete cascade);

create table programs (program_id integer,
                       institution varchar2(100) not null,
                       concentration varchar2(100) not null,
                       degree varchar2(100) not null,
                       primary key (program_id));

create table education (user_id number,
                        program_id integer,
                        program_year integer,
                        primary key (user_id,program_id),
                        foreign key (user_id) references users
                        on delete cascade,
                        foreign key (program_id) references programs
                        on delete cascade);
                        

create table user_events (event_id number,
                          event_creator_id number not null,
                          event_name varchar2(100) not null,
                          event_tagline varchar2(100),
                          event_description varchar2(100),
                          event_host varchar2(100) not null,
                          event_type varchar2(100) not null,
                          event_subtype varchar2(100) not null,
                          event_location varchar2(100),
                          event_city_id integer,
                          event_start_time timestamp not null,
                          event_end_time timestamp not null,
                          primary key (event_id),
                          foreign key (event_creator_id) references users(user_id)
                          on delete cascade,
                          foreign key (event_city_id) references cities(city_id)
                          on delete cascade);

create table participants (event_id number not null,
                           user_id number not null,
                           confirmation varchar2(100) not null,
                           primary key (event_id,user_id),
                           foreign key (event_id) references user_events (event_id)
                           on delete cascade,
--                           on update cascade,
                           foreign key (user_id) references users (user_id)
                           on delete cascade,
                           check (confirmation = 'attending'
                           OR confirmation = 'declined' 
                           OR confirmation = 'unsure' 
                           OR confirmation = 'not-replied'));

create table albums (album_id varchar2(100),
                     album_owner_id number not null,
                     album_name varchar2(100) not null,
                     album_created_time timestamp not null,
                     album_modified_time timestamp not null,
                     album_link varchar2(2000) not null,
                     album_visibility varchar2(100) not null,
                     cover_photo_id varchar2(100) not null,
                     primary key (album_id),
                     foreign key (album_owner_id) references users
                     on delete cascade,
                     check (album_visibility = 'EVERYONE' 
                      OR album_visibility = 'FRIENDS_OF_FRIENDS'
                      OR album_visibility = 'FRIENDS'
                      OR album_visibility = 'MYSELF'
                      OR album_visibility = 'CUSTOM'));

create table photos (photo_id varchar2(100),
                     album_id varchar2(100) not null,
                     photo_caption varchar2(2000),
                     photo_created_time timestamp not null,
                     photo_modified_time timestamp not null,
                     photo_link varchar2(2000) not null,
                     primary key (photo_id));

alter table albums add foreign key (cover_photo_id) references photos initially deferred deferrable;
alter table photos add foreign key (album_id) references albums initially deferred deferrable;

create table tags (tag_photo_id varchar2(100),
                   tag_subject_id number not null,
                   tag_created_time timestamp not null,
                   tag_x number not null,
                   tag_y number not null,
                   primary key (tag_photo_id,tag_subject_id),
                   foreign key (tag_photo_id) references photos(photo_id)
                   on delete cascade,
                   foreign key (tag_subject_id) references users(user_id)
                   on delete cascade);

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

-- Trigger for friends
create or replace trigger fri_trigger 
before insert on friends
for each row when (new.user1_id >= new.user2_id)
begin
:new.user1_ID := :new.user2_id + :new.user1_id;
:new.user2_ID := :new.user1_id - :new.user2_id;
:new.user1_ID := :new.user1_id - :new.user2_id;
end;
/                    

/*
-- Trigger for album_visibility
create or replace trigger alb_vis
before insert on albums
for each row 
begin
if :new.album_visibility <> 'EVERYONE' 
AND :new.album_visibility <> 'FRIENDS_OF_FRIENDS'
AND :new.album_visibility <> 'FRIENDS'
AND :new.album_visibility <> 'MYSELF'
AND :new.album_visibility <> 'CUSTOM' 
then 
RAISE_APPLICATION_ERROR(-20001, 'Insertion Failed');
end if;
end;
/
*/
