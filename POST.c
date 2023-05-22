#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <jansson.h>
#include <string.h>
#include <fcgi_stdio.h>



typedef struct {
    int id;
    char* name;
    char* gender;
    double height;
    double weight;
} Person;


void create_json_array(Person person, double bmi) {
    json_t *arr = json_array();
    json_t *obj = json_object();
    json_t *parent = json_object();

    char *response_data;

    json_object_set_new(obj, "id", json_integer(person.id));
    json_object_set_new(obj, "name", json_string(person.name));
    json_object_set_new(obj, "gender", json_string(person.gender));
    json_object_set_new(obj, "height", json_real(person.height));
    json_object_set_new(obj, "weight", json_real(person.weight));
    json_object_set_new(obj, "bmi", json_real(bmi));

    json_array_append_new(arr, obj);

    json_object_set_new(parent, "status", json_string("success"));
    json_object_set_new(parent, "message", json_string("Successfully calculated BMI."));
    json_object_set_new(parent, "data", arr);

    response_data = json_dumps(parent,  JSON_INDENT(4));

    printf("Content-Type: application/json\n\n");
    printf("%s", response_data);
   
    json_decref(arr);
}

void create_json_error_responses(const char *err_msg, const char *err_msg_add) {
    json_t *parent = json_object();
    json_object_set_new(parent, "status", json_string("error"));
    json_object_set_new(parent, "message", json_string(err_msg));
    json_object_set_new(parent, "message_additional", json_string(err_msg_add));
    printf("Content-Type: application/json\n\n");
    printf("%s", json_dumps(parent,  JSON_INDENT(4)));
}

void loadJSON(Person *person) {

    long len = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
    char* incoming_data = malloc(len+1);

    json_error_t error;

    
    json_t *root;
    json_t *value;
    size_t index;

   
    fgets(incoming_data, len + 1, stdin);

   
    root = json_loads(incoming_data, 0, &error);

    if(!root) {
        create_json_error_responses(error.text, NULL);
    }

    const char* properties[] = { "id", "name", "gender", "height", "weight" };
    char** infovar[] = {&person->name, &person->gender, NULL, NULL};
   
    int num_properties = sizeof(properties) / sizeof(properties[0]);

   
    json_array_foreach(root, index, value) {
        const char *name_holder = json_string_value(json_object_get(value, "name"));
        const char *value_holder = json_string_value(json_object_get(value, "value"));
        int value_holder_length = strlen(value_holder) + 1;

        for (int i = 0; i < num_properties; i++) {
            if (strcmp(name_holder, properties[i]) == 0) {
                if (i == 0) {
                   
                    person->id = atoi(value_holder);
                } else {
                   
                    *infovar[i - 1] = malloc(value_holder_length);
                    strncpy(*infovar[i - 1], value_holder, value_holder_length);
                }
            }
        }
    }

    free(incoming_data);

}

double calculate_bmi(double height, double weight) {
   
    double height_m = height / 100.0;
    return weight / (height_m * height_m);
}

int main(void) {
 while (FCGI_Accept() >= 0) {

        Person person;

        sqlite3 *db;
        sqlite3_stmt *stmt;
        int dbres;

        const char *sql_statement = "insert into info values(?,?,?,?,?);";


       
       
        loadJSON(&person);

       
        double bmi = calculate_bmi(person.height, person.weight);


        dbres = sqlite3_open_v2("db/data.db", &db, SQLITE_OPEN_READWRITE,NULL);

        if (dbres != SQLITE_OK) {
            create_json_error_responses(sqlite3_errmsg(db),NULL);
            sqlite3_close(db);
            return 1;
        }

        dbres = sqlite3_prepare_v2(db, sql_statement, -1, &stmt, NULL);

        dbres = sqlite3_bind_int(stmt, 1, person.id);
        dbres |= sqlite3_bind_text(stmt, 2, person.name, -1, SQLITE_TRANSIENT);
        dbres |= sqlite3_bind_text(stmt, 3, person.gender, -1, SQLITE_TRANSIENT);
        dbres |= sqlite3_bind_double(stmt, 4, person.height);
        dbres |= sqlite3_bind_double(stmt, 5, person.weight);

        if (dbres != SQLITE_OK) {
            create_json_error_responses(sqlite3_errmsg(db),NULL);
            sqlite3_close(db);
            return 1;
        } else {


            char *expanded_sql = sqlite3_expanded_sql(stmt);
            char *err_msg = 0;

            dbres = sqlite3_exec(db, expanded_sql, 0, 0,&err_msg);

            sqlite3_finalize(stmt);

            if (dbres == SQLITE_OK) {
                create_json_array(person, bmi);
                sqlite3_close(db);
                return 1;
            } else {
                create_json_error_responses(sqlite3_errmsg(db),expanded_sql);
                sqlite3_close(db);
                return 1;

            }
        }
        return 0;
    }
}
