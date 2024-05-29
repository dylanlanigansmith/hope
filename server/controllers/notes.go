package controllers

import (
    "encoding/json"
    "net/http"
    "hope/models"
)

func GetNotes(w http.ResponseWriter, r *http.Request) {
    notes, err := models.GetAllNotes()
    if err != nil {
        http.Error(w, err.Error(), http.StatusInternalServerError)
        return
    }
    json.NewEncoder(w).Encode(notes)
}

func CreateNote (w http.ResponseWriter, r *http.Request) {
    var note models.Note
    json.NewDecoder(r.Body).Decode(&note)
    if err := models.CreateNote(&note); err != nil {
        http.Error(w, err.Error(), http.StatusInternalServerError)
        return
    }
    w.WriteHeader(http.StatusCreated)
}



func CreateUser (w http.ResponseWriter, r *http.Request) { 

}

func Login (w http.ResponseWriter, r *http.Request) { 

}