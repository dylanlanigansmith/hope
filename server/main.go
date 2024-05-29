package main

import (
    "log"
    "net/http"
    "hope/controllers"
    "hope/database"
    "github.com/gorilla/mux"
)

//this is gpt crap!!

func main() {
    router := mux.NewRouter()
    database.Connect()

    router.HandleFunc("/api/notes", controllers.GetNotes).Methods("GET")
    router.HandleFunc("/api/notes", controllers.CreateNote).Methods("POST")
    router.HandleFunc("/api/users", controllers.CreateUser).Methods("POST")
    router.HandleFunc("/api/login", controllers.Login).Methods("POST")

    log.Fatal(http.ListenAndServe(":8000", router))
}

