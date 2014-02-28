package main

import (
	"io/ioutil"
	"net/http"
	"html/template"
	"regexp"
	"errors"
)

type Article struct {
	Title string
	Body []byte
}

var templates = template.Must(template.ParseFiles("edit.html", "view.html"))
var validPath = regexp.MustCompile("^/(edit|save|view)/([a-zA-Z0-9]+)$")

func (a *Article) save() error {
	filename := a.Title + ".txt"
	return ioutil.WriteFile(filename, a.Body, 0600)
}

func load(title string) (*Article, error) {
	filename:= title + ".txt"
	body, err := ioutil.ReadFile(filename)
	if err != nil {
		return nil, err
	}
	return &Article{Title: title, Body: body}, nil
}

func renderTemplate(w http.ResponseWriter, tmpl string, a *Article) {
	err := templates.ExecuteTemplate(w, tmpl+".html", a)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}
}

func getTitle(w http.ResponseWriter, r *http.Request) (string, error) {
	m := validPath.FindStringSubmatch(r.URL.Path)
	if m == nil {
		http.NotFound(w, r)
		return "", errors.New("Invalid Article Title")
	}
	return m[2], nil
}

func viewHandler(w http.ResponseWriter, r*http.Request, title string) {
	a, err := load(title)
	if err != nil {
		http.Redirect(w, r, "/edit/"+title, http.StatusFound)
    return
	}
	renderTemplate(w, "view", a)
}

func editHandler(w http.ResponseWriter, r*http.Request, title string) {
	a, err := load(title)
	if err != nil {
		a = &Article{Title: title}
	}
	renderTemplate(w, "edit", a)
}

func saveHandler(w http.ResponseWriter, r *http.Request, title string) {
	body := r.FormValue("body")
	a := &Article{Title: title, Body: []byte(body)}
	err := a.save()
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
    return
	}
	http.Redirect(w, r, "/view/"+title, http.StatusFound)
}

func makeHandler(fn func (http.ResponseWriter, *http.Request, string)) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		m := validPath.FindStringSubmatch(r.URL.Path)
		if m == nil {
			http.NotFound(w, r)
			return
		}
		fn(w, r, m[2])
	}
}

func main() {
	http.HandleFunc("/view/", makeHandler(viewHandler))
	http.HandleFunc("/edit/", makeHandler(editHandler))
	http.HandleFunc("/save/", makeHandler(saveHandler))
	http.ListenAndServe(":9999", nil)
}