package database

import (
    "database/sql"
    _ "github.com/lib/pq"
    "log"
)

var DB *sql.DB

func Connect() {
    var err error
    connStr := "user=hope password=assword dbname=hope sslmode=disable"
    DB, err = sql.Open("postgres", connStr)
    if err != nil {
        log.Fatal(err)
    }

    if err = DB.Ping(); err != nil {
        log.Fatal(err)
    }

    log.Println("Database connected")
}
