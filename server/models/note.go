package models

import (
    "hope/database"
)

type Note struct {
    ID      int    `json:"id"`
    Title   string `json:"title"`
    Content string `json:"content"`
}

func GetAllNotes() ([]Note, error) {
    rows, err := database.DB.Query("SELECT id, title, content FROM notes")
    if err != nil {
        return nil, err
    }
    defer rows.Close()

    var notes []Note
    for rows.Next() {
        var note Note
        if err := rows.Scan(&note.ID, &note.Title, &note.Content); err != nil {
            return nil, err
        }
        notes = append(notes, note)
    }
    return notes, nil
}

func CreateNote (note *Note) error {
    _, err := database.DB.Exec("INSERT INTO notes (title, content) VALUES ($1, $2)", note.Title, note.Content)
    return err
}
