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
                      foreign key (user1_id) references users
                      on delete cascade,
                      foreign key (user2_id) references users
                      on delete cascade);

-- If A and B are friends with each other, and A quits fakebook because he believes it sucks, then A and B should no longer be defined as friends in fakebook.
-- It seems illegal to "change" your friend from C to D. Thus change a user_id is not allowed.

create table cities (city_id integer,
                     city_name varchar2(100),
                     state_name varchar2(100),
                     country_name varchar2(100),
                     primary key (city_id));

create table user_current_city (user_id number,
                                current_city_id integer,
                                primary key (user_id, current_city_id),
                                foreign key (user_id) references users
                                on delete cascade,
                                foreign key (current_city_id) references cities
                                on delete cascade);

-- 

create table user_hometown_city (user_id number,
                                 hometown_city_id integer,
                                 primary key (user_id, hometown_city_id),
                                 foreign key (user_id) references users
                                 on delete cascade,
                                 foreign key (hometown_city_id) references cities
                                 on delete cascade);
                                 --on update cascade);

create table message (message_id integer,
                      sender_id number not null,
                      receiver_id number not null,
                      message_content varchar2(2000),
                      sent_time timestamp not null,
                      primary key (message_id),
                      foreign key (sender_id) references users
                      on delete cascade,
                      foreign key (receiver_id) references users
                      on delete cascade);

create table programs (program_id integer,
                       institiution varchar2(100),
                       concentration varchar2(100),
                       degree varchar2(100),
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
                          event_host varchar2(100),
                          event_type varchar2(100),
                          event_subtype varchar2(100),
                          event_location varchar2(100),
                          event_city_id integer,
                          event_start_time timestamp not null,
                          event_end_time timestamp not null,
                          primary key (event_id),
                          foreign key (event_creator_id) references users
                          on delete cascade);

create table participants (event_id number,
                           user_id number,
                           confirmation varchar2(100) not null,
                           primary key (event_id,user_id),
                           foreign key (event_id) references user_events
                           on delete cascade,
--                           on update cascade,
                           foreign key (user_id) references users
                           on delete cascade);

create table albums (album_id varchar2(100),
                     album_owner_id number,
                     album_name varchar2(100) not null,
                     album_created_time timestamp not null,
                     album_modified_time timestamp not null,
                     album_link varchar2(2000) not null,
                     album_visibility varchar2(100),
                     cover_photo_id varchar2(100),
                     primary key (album_id),
                     foreign key (album_owner_id) references users
                     on delete cascade);

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
                   tag_subject_id number,
                   tag_created_time timestamp not null,
                   tag_x number not null,
                   tag_y number not null,
                   primary key (tag_photo_id,tag_subject_id),
                   foreign key (tag_photo_id) references photos
                   on delete cascade,
                   foreign key (tag_subject_id) references users
                   on delete cascade);

                     


