-- EXERCISE 1

-- Question 1
SELECT movie FROM STARREDIN WHERE CELEB='Tom Cruise';

-- Question 2
SELECT movie FROM STARREDIN WHERE CELEB='Tom Cruise'
INTERSECT
SELECT movie FROM STARREDIN WHERE CELEB='Penelope Cruz';

-- Question 3
SELECT DISTINCT CELEB FROM STARREDIN WHERE MOVIE IN (SELECT MOVIE FROM STARREDIN WHERE CELEB='Tom Cruise')
MINUS
SELECT DISTINCT CELEB FROM STARREDIN WHERE CELEB='Tom Cruise';

-- Question 4
SELECT CELEB as costar, MOVIE
FROM STARREDIN JOIN
      (SELECT CELEB1 as costar1
      FROM RELATIONSHIPS
      WHERE CELEB2 = 'Tom Cruise'
      UNION
      SELECT CELEB2 as costar1
      FROM RELATIONSHIPS
      WHERE CELEB1 = 'Tom Cruise') ON
     STARREDIN.CELEB = costar1
WHERE MOVIE IN
  (SELECT MOVIE AS X
   FROM STARREDIN
   WHERE STARREDIN.CELEB = 'Tom Cruise');

-- Question 5
SELECT CELEB1, CELEB2, MOVIE
FROM (RELATIONSHIPS JOIN STARREDIN
  ON RELATIONSHIPS.CELEB1 = STARREDIN.CELEB)
INTERSECT
SELECT CELEB1, CELEB2, MOVIE
FROM (RELATIONSHIPS JOIN STARREDIN
  ON RELATIONSHIPS.CELEB2 = STARREDIN.CELEB);

-- Question 6
SELECT *
FROM (
  SELECT CELEB, MOVIES
  FROM (SELECT CELEB, COUNT(MOVIE) AS MOVIES
        FROM STARREDIN
        GROUP BY CELEB)
  WHERE MOVIES >= 3)
ORDER BY MOVIES DESC;

-- Question 7
SELECT X.CELEB2 AS CELEB1, Y.CELEB2 AS CELEB2, X.CELEB1 AS CELEB3
FROM RELATIONSHIPS X JOIN RELATIONSHIPS Y ON
  X.CELEB1 = Y.CELEB1 AND X.CELEB2 <> Y.CELEB2;

-- Question 8
CREATE VIEW celebMovieCounts AS
  SELECT CELEB, MOVIES
  FROM (SELECT CELEBS.NAME AS CELEB, COUNT(MOVIE) AS MOVIES
        FROM STARREDIN X RIGHT JOIN CELEBS ON CELEBS.NAME = X.CELEB
        GROUP BY CELEBS.NAME);

SELECT ENEMIES.CELEB1, ENEMIES.CELEB2, X.MOVIES AS n1, Y.MOVIES AS n2
FROM ENEMIES, CELEBMOVIECOUNTS X, CELEBMOVIECOUNTS Y
WHERE ENEMIES.CELEB1 = X.CELEB AND ENEMIES.CELEB2 = Y.CELEB;


-- Question 9
SELECT CELEB, ALBUMS
FROM (
  SELECT X.CELEB, X.ALBUMS
  FROM (SELECT CELEB, COUNT(ALBUM) AS ALBUMS
        FROM RELEASES
        GROUP BY CELEB) X JOIN CELEBMOVIECOUNTS Y ON
          X.CELEB = Y.CELEB
  WHERE Y.MOVIES >= 2)
ORDER BY ALBUMS DESC;

-- Question 10
SELECT DISTINCT CELEB FROM STARREDIN
INTERSECT
SELECT DISTINCT CELEB FROM RELEASES;

-- Question 11
CREATE VIEW celebAlbumCounts AS
  SELECT CELEB, ALBUMS
  FROM (SELECT CELEB, COUNT(ALBUM) AS ALBUMS
        FROM RELEASES
        GROUP BY CELEB);
        
SELECT celeb.CELEB, albums.albums, movies.movies
FROM (SELECT DISTINCT X.CELEB
     FROM RELEASES, STARREDIN X
     WHERE RELEASES.CELEB = X.CELEB) celeb, CELEBALBUMCOUNTS albums, CELEBMOVIECOUNTS movies
WHERE celeb.CELEB = albums.CELEB AND celeb.CELEB = movies.CELEB;

-- Question 12
SELECT * 
FROM RELATIONSHIPS
WHERE STARTED = (SELECT MIN(STARTED)
                 FROM RELATIONSHIPS);
                 
SELECT * 
FROM RELATIONSHIPS
WHERE STARTED = (SELECT MAX(STARTED)
                 FROM RELATIONSHIPS);

-- Question 13
INSERT INTO RELATIONSHIPS (CELEB1, CELEB2) 
  SELECT X.CELEB2 AS FCELEB1, X.CELEB1 AS FCELEB2
  FROM RELATIONSHIPS X
  WHERE (X.CELEB2, X.CELEB1) NOT IN (
    SELECT Y.CELEB1, Y.CELEB2
    FROM RELATIONSHIPS Y);

ROLLBACK;

-- Question 14
DELETE FROM STARREDIN X WHERE X.MOVIE IN
  (SELECT MOVIE
  FROM (
    SELECT COUNT(CELEB) AS STARS, MOVIE
    FROM STARREDIN
    GROUP BY MOVIE)
  WHERE STARS < 2);
  
ROLLBACK;

-- END OF EXERCISE 1



---------------------------------------------------



-- EXERCISE 2

-- Question 1
CREATE TABLE complexes
(
 cid int NOT NULL PRIMARY KEY,
 location varchar(30),
 area varchar(30)
);

CREATE TABLE multisport
(
 cid int,
 FOREIGN KEY (cid) REFERENCES complexes(cid)
);

CREATE TABLE sports
(
  name varchar(25) NOT NULL PRIMARY KEY
);

CREATE TABLE designatedfor
(
 cid int,
 sportname varchar(25),
 locIndicator varchar(20),
 FOREIGN KEY (cid) REFERENCES complexes(cid),
 FOREIGN KEY (sportname) REFERENCES sports(name)
);

CREATE TABLE onesport
(
 cid int,
 sportname varchar(25),
 FOREIGN KEY (cid) REFERENCES complexes(cid),
 FOREIGN KEY (sportname) REFERENCES sports(name)
);

CREATE TABLE officials
(
 oid int NOT NULL PRIMARY KEY,
 name varchar(30)
);

CREATE TABLE events
(
 eventid int NOT NULL PRIMARY KEY,
 e_date DATE,
 np int,
 chief_org_oid int,
 cid int,
 FOREIGN KEY (chief_org_oid) REFERENCES officials(oid),
 FOREIGN KEY (cid) REFERENCES complexes(cid)
);

CREATE TABLE involved
(
 oid int,
 eventid int,
 FOREIGN KEY (oid) REFERENCES officials(oid),
 FOREIGN KEY (eventid) REFERENCES events(eventid)
);

CREATE TABLE equipment
(
 equipid int NOT NULL PRIMARY KEY,
 description varchar(30)
);

CREATE TABLE usedfor
(
 equipid int,
 eventid int,
 FOREIGN KEY (equipid) REFERENCES equipment(equipid),
 FOREIGN KEY (eventid) REFERENCES events(eventid)
);

-- Question 2
INSERT INTO complexes VALUES (1, 'Victoria', 'Dowtown');
INSERT INTO multisport VALUES (1);
INSERT INTO sports VALUES ('Hockey');
INSERT INTO designatedfor VALUES(1, 'Hockey', 'North West');
INSERT INTO onesport VALUES(1, 'Hockey');
INSERT INTO officials VALUES (1, 'Jordan');
INSERT INTO events VALUES (1, CURRENT_DATE, 1, 1, 1);
INSERT INTO involved VALUES (1, 1);
INSERT INTO equipment VALUES (1, 'Hockey skates');
INSERT INTO usedfor VALUES (1, 1);

-- Question 3
DROP TABLE designatedfor;
DROP TABLE onesport;
DROP TABLE multisport;
DROP TABLE sports;
DROP TABLE involved;
DROP TABLE usedfor;
DROP TABLE events;
DROP TABLE officials;
DROP TABLE equipment;
DROP TABLE complexes;

-- Question 1
CREATE TABLE departments
(
 code int NOT NULL PRIMARY KEY,
 name varchar(30),
 phone varchar(10)
);

CREATE TABLE students
(
 name varchar(30),
 snumber int NOT NULL PRIMARY KEY,
 code int,
 dob DATE,
 FOREIGN KEY (code) REFERENCES departments(code)
);

CREATE TABLE terms
(
 termid int NOT NULL PRIMARY KEY
);

CREATE TABLE courses
(
 name varchar(30) NOT NULL PRIMARY KEY,
 description varchar(50),
 location varchar(50)
);

CREATE TABLE offered
(
 code int,
 name varchar(30),
 FOREIGN KEY (code) REFERENCES departments(code),
 FOREIGN KEY (name) REFERENCES courses(name)
);

CREATE TABLE registered
(
 snumber int,
 name varchar(30),
 term int,
 grade int,
 FOREIGN KEY (snumber) REFERENCES students(snumber),
 FOREIGN KEY (name) REFERENCES courses(name)
);

-- Question 2
INSERT INTO departments VALUES (1, 'Computer Science', '2505551834');
INSERT INTO students VALUES ('Jordan Ell', 1, 1, CURRENT_DATE);
INSERT INTO terms VALUES (1);
INSERT INTO courses VALUES ('CSC 370', 'This is the databases course', 'Cornett');
INSERT INTO offered VALUES (1, 'CSC 370');
INSERT INTO registered VALUES (1, 'CSC 370', 4, 99);

-- Question 3
DROP TABLE offered;
DROP TABLE registered;
DROP TABLE students;
DROP TABLE terms;
DROP TABLE courses;
DROP TABLE departments;

-- END OF EXERCISE 2
