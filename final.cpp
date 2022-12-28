#include <iostream>
#include <map>
#include <vector>
#include <cstring>
#include <tuple>
#include <limits>
using namespace std;

class Database;
class Visitor
{
    public:
        virtual~Visitor() {}

    virtual void visit(Database *const db) = 0;
};

class Database
{
    public:
        ~Database() {}

    map<string, string> user_map = {};
    map<string, vector<vector<float>>> sheet_data_map = {};
    map<string, map<string, bool>> sheet_permission_map = {};

    void accept(Visitor & visitor)
    {
        visitor.visit(this);
    };

    void create_user(string user_name)
    {
        if (check_user_exist(user_name)) throw invalid_argument( "User is already exist!!!\n" );
        
        user_map.insert({ user_name, user_name });
    };

    void create_sheet(string user_name, string sheet_name)
    {
        if (!check_user_exist(user_name)) throw invalid_argument( "User is not exist!!!\n" );
        
        if (check_sheet_exist(sheet_name)) throw invalid_argument("Sheet is already exist!!!\n" );
        
        vector<float> row(3, 0);
        vector<vector<float>> data(3, row);
        sheet_data_map.insert({ sheet_name, data });
        
        map<string, bool> m;
        m.insert({ user_name, true });
        sheet_permission_map.insert({ sheet_name, m });
    };

    vector<vector<float>> get_sheet(string user_name, string sheet_name)
    {
        if (!check_user_exist(user_name)) throw invalid_argument( "User is not exist!!!\n" );
        
        if (!check_sheet_exist(sheet_name)) throw invalid_argument( "Sheet is not exist!!!\n" );
        
        if (!check_readable(user_name, sheet_name)) throw invalid_argument( "This sheet is not accessible.\n" );

        return sheet_data_map[sheet_name];
    };
    
    vector<vector<float>> update_sheet(string user_name, string sheet_name, int x, int y, float new_value)
    {
        if (!check_user_exist(user_name)) throw invalid_argument( "User is not exist!!!\n" );
        
        if (!check_sheet_exist(sheet_name)) throw invalid_argument( "Sheet is not exist!!!\n" );
        
        if (!check_editable(user_name, sheet_name)) throw invalid_argument( "This sheet is not accessible.\n" );
        
        if (x > 2 || y > 2) throw invalid_argument( "Invalid range!!!\n" );

        vector<vector<float>> data = sheet_data_map[sheet_name];
        data[x][y] = new_value;
        sheet_data_map[sheet_name] = data;
        return data;
    };

    void update_permission(string user_name, string sheet_name, bool editable)
    {
        map<string, bool> permission_map = sheet_permission_map[sheet_name];
        permission_map[user_name] = editable;
        sheet_permission_map[sheet_name] = permission_map;
    }

    int add_collaborate_user(string user_name, string sheet_name, string another_user_name)
    {
        if (!check_user_exist(user_name)) throw invalid_argument( "User is not exist!!!\n" );
        
        if (!check_sheet_exist(sheet_name)) throw invalid_argument( "Sheet is not exist!!!\n" );
        
        if (!check_editable(user_name, sheet_name)) throw invalid_argument( "This sheet is not accessible.\n" );
        
        if (!check_user_exist(another_user_name)) throw invalid_argument( "User is not exist!!!\n" );

        map<string, bool> permission_map = sheet_permission_map[sheet_name];
        if (permission_map.find(another_user_name) == permission_map.end())
        {
            permission_map.insert({ another_user_name, true });
        }
        else
        {
            permission_map[another_user_name] = true;
        }

        sheet_permission_map[sheet_name] = permission_map;
        return 0;
    }

    bool update_sheet(string user_name, string sheet_name, int x, int y, int new_value)
    {
        return false;
    };

    private:
        bool check_editable(string user_name, string sheet_name)
        {
            map<string, bool> permission_map = sheet_permission_map[sheet_name];

            return permission_map[user_name];
        }

        bool check_readable(string user_name, string sheet_name)
        {
            map<string, bool> permission_map = sheet_permission_map[sheet_name];

            return key_exist_in_map(permission_map, user_name);
        }
        
        bool check_user_exist(string user_name)
        {
            return key_exist_in_map(user_map, user_name);
        }
    
        bool check_sheet_exist(string sheet_name)
        {
            return key_exist_in_map(sheet_permission_map, sheet_name);
        }

        bool key_exist_in_map(map<string, string> m, string k) {
            return m.find(k) != m.end();
        };
    
        bool key_exist_in_map(map<string, bool> m, string k) {
            return m.find(k) != m.end();
        };
    
        bool key_exist_in_map(map<string, map<string, bool>> m, string k) {
            return m.find(k) != m.end();
        };
};

class CreateUserVisitor: public Visitor
{
    public:
        ~CreateUserVisitor() {}

    void visit(Database *db)
    {
        string user_name;
        cout << "> ";
        cin >> user_name;
        try {
            db->create_user(user_name);
            cout << "Create a user named " << "\"" << user_name << "\"" << ".\n" << endl;
        } catch( const std::invalid_argument& e ) {
            cout << e.what() << endl;
        }
    }
};

class CreateSheetVisitor: public Visitor
{
    public:
        ~CreateSheetVisitor() {}

    void visit(Database *db)
    {
        string user_name, sheet_name;
        cout << "> ";
        cin >> user_name >> sheet_name;
        try {
            db->create_sheet(user_name, sheet_name);
            cout << "Create a sheet named " << "\"" << sheet_name << "\"" << " for " << "\"" << user_name << "\"" << ".\n" << endl;
        } catch( const std::invalid_argument& e ) {
            cout << e.what() << endl;
        }
    }
};

class CheckSheetVisitor: public Visitor
{
    public:
        ~CheckSheetVisitor() {}

    void visit(Database *db)
    {
        string user_name, sheet_name;
        cout << "> ";
        cin >> user_name >> sheet_name;
        try {
            vector<vector<float>> data = db->get_sheet(user_name, sheet_name);
            cout << "\n";
            print_data(data);
        } catch( const std::invalid_argument& e ) {
            cout << e.what() << endl;
        }
    }

    void print_data(vector<vector<float>> data)
    {
        for ( const auto &row : data )
        {
           for ( const auto &s : row ) std::cout << s << ' ';
           std::cout << std::endl;
        }
        cout << "\n";
    }
};

class UpdateSheetVisitor: public Visitor
{
    public:
        ~UpdateSheetVisitor() {}

    void visit(Database *db)
    {
        string user_name, sheet_name, expression;
        int x, y;
        float new_value;
        
        cout << "> ";
        cin >> user_name >> sheet_name;
        try {
            vector<vector<float>> data = db->get_sheet(user_name, sheet_name);
            cout << "\n";
			print_data(data);
        } catch( const std::invalid_argument& e ) {
            cout << e.what() << endl;
        }
        cout << "> ";
        cin >> x >> y >> expression;
        new_value = eval_expression(expression);
        try {
            vector<vector<float>> data = db->update_sheet(user_name, sheet_name, x, y, new_value);
            print_data(data);
        } catch( const std::invalid_argument& e ) {
            cout << e.what() << endl;
        }
        
    }

    void print_data(vector<vector<float>> data)
    {
        for ( const auto &row : data )
        {
           for ( const auto &s : row ) std::cout << s << ' ';
           std::cout << std::endl;
        }
        cout << "\n";
    }
    
    float eval_expression(string expression) {
        float f1, f2;
        if (expression.find('+') != std::string::npos) {
            tie(f1, f2) = get_numbers(expression, "+");
            return f1 + f2;
        } else if (expression.find('-') != std::string::npos) {
            tie(f1, f2) = get_numbers(expression, "-");
            return f1 - f2;
        } else if (expression.find('*') != std::string::npos) {
            tie(f1, f2) = get_numbers(expression, "*");
            return f1 * f2;
        } else if (expression.find('/') != std::string::npos) {
            tie(f1, f2) = get_numbers(expression, "/");
            return f1 / f2;
        }
        
        return stof(expression);
    }
    
    tuple<float, float> get_numbers(string expression, string str_operator) {
        size_t pos = expression.find(str_operator);
        string str1 = expression.substr(0, pos);
        string str2 = expression.substr(pos + 1);
        float f1 = stof(str1);
        float f2 = stof(str2);
        
        return { f1, f2 };
    }
};

class UpdatePermissionVisitor: public Visitor
{
    public:
        ~UpdatePermissionVisitor() {}

    void visit(Database *db)
    {
        string user_name, sheet_name, permission;
        cout << "> ";
        cin >> user_name >> sheet_name >> permission;
        if (permission == "ReadOnly")
        	db->update_permission(user_name, sheet_name, permission == "Editable");
        else if (permission == "Editable")
        	db->update_permission(user_name, sheet_name, permission == "Editable");
        else throw invalid_argument( "Invalid input!\n" );
        cout << endl;
    }
};

class CollaborateSheetVisitor: public Visitor
{
    public:
        ~CollaborateSheetVisitor() {}

    void visit(Database *db)
    {
        string user_name, sheet_name, another_user_name;
        cout << "> ";
        cin >> user_name >> sheet_name >> another_user_name;
        try {
            db->add_collaborate_user(user_name, sheet_name, another_user_name);
            cout << "Share \"" << user_name << "\"'s \"" << sheet_name << "\" with \"" << another_user_name << "\"." << endl;
        } catch( const std::invalid_argument& e ) {
            cout << e.what() << endl;
        }
    }
};

string menu()
{
    string c_num;
    cout << "---------------Menu---------------" << endl;
    cout << "1. Create a user." << endl;
    cout << "2. Create a sheet." << endl;
    cout << "3. Check a sheet." << endl;
    cout << "4. Change a value in a sheet." << endl;
    cout << "5. Change a sheet's access right. (ReadOnly/Editable)" << endl;
    cout << "6. Collaborate with an other user." << endl;
	cout << "Please enter" << " \"exit\" " << "if you want to leave." << endl; 
    cout << "----------------------------------" << endl;
    cout << "> ";
    cin >> c_num;
    return c_num;
};

int main()
{
    string c_num;
    string user_name, sheet_name;
    Database db;
    CreateUserVisitor createUserVisitor;
    CreateSheetVisitor createSheetVisitor;
    CheckSheetVisitor checkSheetVisitor;
    UpdateSheetVisitor updateSheetVisitor;
    UpdatePermissionVisitor updatePermissionVisitor;
    CollaborateSheetVisitor collaborateSheetVisitor;
    c_num = menu();
    while (c_num != "exit")
    {
        if (c_num == "1"){
            db.accept(createUserVisitor);
            c_num = "0";
        }
        
        else if (c_num == "2"){
            db.accept(createSheetVisitor);
            c_num = "0";
        }
        
        else if (c_num == "3"){
            db.accept(checkSheetVisitor);
            c_num = "0";
        }
        
        else if (c_num == "4"){
            db.accept(updateSheetVisitor);
            c_num = "0";
        }
        
        else if (c_num == "5"){
            db.accept(updatePermissionVisitor);
            c_num = "0";
        }
        
        else if (c_num == "6"){
            db.accept(collaborateSheetVisitor);
            c_num = "0";
        }
        else{
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            c_num = menu();
        }
    }

    return 0;
}
