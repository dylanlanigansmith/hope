#!/bin/bash


#i dont run this by default
#bad vibes running sudo in a build script
#sorry not sorry
sudo systemctl start docker  

docker pull postgres

docker run --name postgres-container -e POSTGRES_DB=hope -e POSTGRES_USER=hope -e POSTGRES_PASSWORD=assword -p 5432:5432 -d postgres


#docker exec -it postgres-container psql -U hope -d hope