-- Part 3 Database loading

-- User data loading
insert into users (user_id, first_name, last_name, year_of_birth, month_of_birth, day_of_birth, gender) 
select distinct user_id, first_name, last_name, year_of_birth, month_of_birth, day_of_birth, gender from weile.public_user_information;

-- Friends data loading
insert into friends (user1_id, user2_id)
select distinct user1_id, user2_id from weile.public_are_friends;

-- Cities data loading
insert into cities (city_name, state_name, country_name)
select distinct hometown_city, hometown_state, hometown_country from weile.public_user_information 
union
select distinct current_city, current_state, current_country from weile.public_user_information
union
select distinct event_city, event_state, event_country from weile.public_event_information;


-- current_city loading
insert into user_current_city (user_id, current_city_id)
select distinct users.user_id, cities.city_id from users, cities, weile.public_user_information pubData 
where users.user_id = pubData.user_id 
and cities.city_name = pubData.current_city 
and cities.state_name = pubData.current_state
and cities.country_name = pubData.current_country;

-- hometown loading
insert into user_hometown_city (user_id, hometown_city_id)
select distinct users.user_id, cities.city_id from users, cities, weile.public_user_information pubData 
where users.user_id = pubData.user_id 
and cities.city_name = pubData.hometown_city
and cities.state_name = pubData.hometown_state
and cities.country_name = pubData.hometown_country;

-- Programs loading
insert into programs (institution, concentration, degree)
select distinct institution_name, program_concentration, program_degree from weile.public_user_information 
where institution_name is not null
and program_concentration is not null
and program_degree is not null;

-- Education loading
insert into education (user_id, program_id, program_year)
select distinct users.user_id, prog.program_id, pubData.program_year from users, programs prog, weile.public_user_information pubData 
where users.user_id = pubData.user_id 
and prog.institution = pubData.institution_name
and prog.concentration = pubData.program_concentration
and prog.degree = pubData.program_degree;

-- Events data loading
/* OLD VERSION
insert into user_events (event_id, event_creator_id, event_name, event_tagline, event_description, event_host, event_type, event_subtype, event_location, event_city_id, event_start_time, event_end_time) 
select distinct event_id, event_creator_id, event_name, event_tagline, event_description, event_host, event_type, event_subtype, event_location, cities.city_id, event_start_time, event_end_time from weile.public_event_information pubEvent, cities
where  pubEvent.event_city = cities.city_name
and pubEvent.event_state = cities.state_name
and pubEvent.event_country = cities.country_name;
*/
insert into user_events (event_id, event_creator_id, event_name, event_tagline, event_description, event_host, event_type, event_subtype, event_location, event_city_id, event_start_time, event_end_time) 
select distinct event_id, event_creator_id, event_name, event_tagline, event_description, event_host, event_type, event_subtype, event_location, cities.city_id, event_start_time, event_end_time 
FROM weile.public_event_information pubEvent 
LEFT JOIN  cities ON pubEvent.event_city = cities.city_name
LEFT JOIN  cities ON pubEvent.event_state = cities.state_name
LEFT JOIN  cities ON pubEvent.event_country = cities.country_name;

-- album data loading
set autocommit off
insert into albums (album_id, album_owner_id, album_name, album_created_time, album_modified_time, album_link, album_visibility, cover_photo_id)
select distinct album_id, owner_id, album_name, album_created_time, album_modified_time, album_link, album_visibility, cover_photo_id from weile.public_photo_information;

-- photo data loading
insert into photos (photo_id, album_id, photo_caption, photo_created_time, photo_modified_time, photo_link)
select distinct photo_id, album_id, photo_caption, photo_created_time, photo_modified_time, photo_link from weile.public_photo_information; 
commit
set autocommit on

-- tag info loading
insert into tags (tag_photo_id, tag_subject_id, tag_created_time, tag_x, tag_y)
select distinct photo_id, tag_subject_id, tag_created_time, tag_x_coordinate, tag_y_coordinate from weile.public_tag_information;







