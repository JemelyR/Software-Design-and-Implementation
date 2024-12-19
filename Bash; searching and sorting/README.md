# Jemely Robles
## CS50 Winter 2023, Lab 1

GitHub username: JemelyR

## Part B
'''bash
  ln -s /thayerfs/courses/23winter/cosc50/COVID-19/data_tables/vaccine_data/us_data/hourly/vaccine_data_us.csv
'''

## Part C
'''bash
  sed -n '1p' vaccine_data_us.csv
'''

## Part D
'''bash
  sed -n '1p' vaccine_data_us.csv | tr ',' '\n'
'''

## Part E
'''bash
  find . -name 'vaccine_data_us.csv' -exec grep 'New Hampshire' {} \;
'''

## Part F
'''bash
  find . -name 'vaccine_data_us.csv' -exec grep 'All' {} \; | tr ',' '\t' | cut -f 2,10 | tr -s '\t' ','
'''

## Part G
'''bash
  find . -name 'vaccine_data_us.csv' -exec grep 'All' {} \; | tr ',' '\t' | cut -f 2,10 | tr -s '\t' ',' | sort -t$',' -k2 -nr | head -10
'''

## Part H
'''bash
  find . -name 'vaccine_data_us.csv' -exec grep 'All' {} \; | tr ',' '\t' | cut -f 2,10 | tr -s '\t' ',' | sort -t$',' -k2 -nr | head -10 | tr -s ',' '|' | sed -r 's/(^|$)/|/g'
'''

