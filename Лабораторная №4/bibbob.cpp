#include <iostream>
#include <string>
#include <fstream>
#include <pqxx/pqxx>

using namespace std;

void logAction(const string& text) {
    ofstream log("log.txt", ios::app);
    log << text << endl;
}

class Product {
protected:
    string name;
    double price;
public:
    Product(string n, double p) : name(n), price(p) {}
    virtual string getType() { return "Product"; }
};

class DigitalProduct : public Product {
public:
    DigitalProduct(string n, double p) : Product(n, p) {}
    string getType() override { return "Digital"; }
};

class PhysicalProduct : public Product {
public:
    PhysicalProduct(string n, double p) : Product(n, p) {}
    string getType() override { return "Physical"; }
};

template<typename T1, typename T2>
class Pair {
public:
    T1 first;
    T2 second;
    Pair(T1 f, T2 s) : first(f), second(s) {}
};

void addProduct(pqxx::connection& conn) {
    string name;
    double price;

    cout << "Product name: ";
    cin >> name;
    cout << "Price: ";
    cin >> price;

    try {
        pqxx::work txn(conn);
        txn.exec("INSERT INTO products (name, price) VALUES (" +
            txn.quote(name) + ", " + txn.quote(price) + ")");
        txn.commit();

        logAction("Added product: " + name);
        cout << "Product added\n";
    }
    catch (...) {
        cout << "Error adding product\n";
    }
}

void addOrder(pqxx::connection& conn) {
    try {
        pqxx::work txn(conn);
        txn.exec("INSERT INTO orders (order_date) VALUES (CURRENT_DATE)");
        txn.commit();

        logAction("Added order");
        cout << "Order added\n";
    }
    catch (...) {
        cout << "Error adding order\n";
    }
}

void addOrderItem(pqxx::connection& conn) {
    int order_id, product_id, quantity;

    cout << "Order ID: ";
    cin >> order_id;
    cout << "Product ID: ";
    cin >> product_id;
    cout << "Quantity: ";
    cin >> quantity;

    try {
        pqxx::work txn(conn);

        pqxx::result priceRes =
            txn.exec("SELECT price FROM products WHERE id = " + txn.quote(product_id));

        if (priceRes.empty()) {
            cout << "Product not found\n";
            return;
        }

        double price = priceRes[0][0].as<double>();
        double total = price * quantity;

        txn.exec(
            "INSERT INTO order_items (order_id, product_id, quantity, total_price) VALUES (" +
            txn.quote(order_id) + ", " +
            txn.quote(product_id) + ", " +
            txn.quote(quantity) + ", " +
            txn.quote(total) + ")"
        );

        txn.commit();
        logAction("Added order item");
        cout << "Order item added\n";
    }
    catch (...) {
        cout << "Error adding order item\n";
    }
}

void showProducts(pqxx::connection& conn) {
    pqxx::work txn(conn);
    pqxx::result res = txn.exec("SELECT * FROM products");

    for (auto row : res) {
        cout << row["id"].as<int>() << " "
            << row["name"].c_str() << " "
            << row["price"].as<double>() << endl;
    }
}

void showOrders(pqxx::connection& conn) {
    pqxx::work txn(conn);
    pqxx::result res = txn.exec("SELECT * FROM orders");

    for (auto row : res) {
        cout << row["order_id"].as<int>() << " "
            << row["order_date"].c_str() << endl;
    }
}

void revenueByProduct(pqxx::connection& conn) {
    pqxx::work txn(conn);
    pqxx::result res = txn.exec(
        "SELECT p.name, SUM(oi.total_price) AS revenue "
        "FROM products p JOIN order_items oi ON p.id = oi.product_id "
        "GROUP BY p.name"
    );

    for (auto row : res) {
        cout << row["name"].c_str() << " revenue: "
            << row["revenue"].as<double>() << endl;
    }

    logAction("Revenue report");
}

void top3Products(pqxx::connection& conn) {
    pqxx::work txn(conn);
    pqxx::result res = txn.exec(
        "SELECT p.name, SUM(oi.quantity) AS qty "
        "FROM products p JOIN order_items oi ON p.id = oi.product_id "
        "GROUP BY p.name ORDER BY qty DESC LIMIT 3"
    );

    for (auto row : res) {
        cout << row["name"].c_str() << " sold: "
            << row["qty"].as<int>() << endl;
    }

    logAction("Top 3 products");
}

void avgOrderCost(pqxx::connection& conn) {
    pqxx::work txn(conn);
    pqxx::result res = txn.exec(
        "SELECT AVG(sum) FROM ("
        "SELECT SUM(total_price) AS sum FROM order_items GROUP BY order_id"
        ") t"
    );

    cout << "Average order cost: "
        << res[0][0].as<double>() << endl;

    logAction("Average order cost");
}

int main() {
    pqxx::connection conn(
        "dbname=sales_db user=postgres password=ТВОЙ_ПАРОЛЬ host=localhost"
    );

    int choice;
    do {
        cout << "\n1 Add product\n";
        cout << "2 Add order\n";
        cout << "3 Add order item\n";
        cout << "4 Show products\n";
        cout << "5 Show orders\n";
        cout << "6 Revenue by product\n";
        cout << "7 Top 3 products\n";
        cout << "8 Average order cost\n";
        cout << "0 Exit\n";
        cin >> choice;

        switch (choice) {
        case 1: addProduct(conn); break;
        case 2: addOrder(conn); break;
        case 3: addOrderItem(conn); break;
        case 4: showProducts(conn); break;
        case 5: showOrders(conn); break;
        case 6: revenueByProduct(conn); break;
        case 7: top3Products(conn); break;
        case 8: avgOrderCost(conn); break;
        }
    } while (choice != 0);

    return 0;
}
