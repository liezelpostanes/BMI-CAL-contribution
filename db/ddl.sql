CREATE DACREATE DATABASE bmi_calculator;
\c bmi_calculator;

CREATE TABLE bmi_data (
  id SERIAL PRIMARY KEY,
  age INT,
  height INT,
  weight INT,
  gender VARCHAR(10),
  bmi FLOAT,
  result VARCHAR(20)
);
