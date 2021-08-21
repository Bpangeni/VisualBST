#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;

struct Node {
    int data;
    Node *left;
    Node *right;
    Node(int val) {
        data = val;
        left = NULL;
        right = NULL;
    }
};

Node* insertBST(Node *root,int val)
{
    if (root == NULL)
    {
        return new Node(val);
    }

    if(val < root->data)
    {
        root->left = insertBST(root->left, val);
    } else
    {
        // val > root->data;
        root->right = insertBST(root->right,val);
    }
    return root;
}

void inorder(Node *root)
{
    if(root==NULL)
    {
        return;
    }
    inorder(root->left);
    cout << root->data << " ";
    inorder(root->right);
}

Node* searchProcess(Node *root,int key)
{
    if(root == NULL)
    {
        return NULL;
    }
    if(root->data == key)
    {
        return root;
    }
    if (root->data > key)
    {
        return searchProcess(root->left,key);
    }
    return searchProcess(root->right,key);  // if root->data < key
}

void SearchInBST(Node* root, int data)
{   cout << "Searching "<< data << " in BST..." << endl;
    if(searchProcess(root,data))
    {
        cout << "key " << data<<" exists in BST." << endl;
    } else
    {
        cout << "Key " << data<<" doesn't exist in BST." << endl;
    }
}

Node* Inordersuccesor(Node* root) {
    Node* current = root;
    while (current && current->left != NULL)
    {
        current = current->left;
    }
    return current;
}

Node* DeleteInBST(Node *root, int key)
{
    if(key < root->data)
    {
        root->left = DeleteInBST(root->left, key);
    }
    else if(key > root->data)
    {
        root->right = DeleteInBST(root->right,key);
    }
    else {
        if(root->left == NULL)
        {
            Node *temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL)
        {
            Node *temp = root->left;
            free(root);
            return temp;
        }
        //This code is written for the case where the particular node which is to be deleted has two child.
        Node* temp = Inordersuccesor(root->right);
        root->data = temp->data;
        root->right = DeleteInBST(root->right,temp->data);
    }
    return root;
}

template <typename F>
void traverse_inner(Node * node, int xoff, int yoff, int parent_xoff, int parent_yoff, F inspect) {
    inspect(node->data, xoff, yoff, true, parent_xoff, parent_yoff);
    if (node->left) {
        traverse_inner(node->left, xoff - 1, yoff + 1, xoff, yoff, inspect);
    }
    if (node->right) {
        traverse_inner(node->right, xoff + 1, yoff + 1, xoff, yoff, inspect);
    }
}

void traverse_overlap_adjust(Node * node, int & initial_left, int & initial_right) {
    if (node->left == nullptr || node->right == nullptr) {
        return;
    }
    traverse_inner(node->left, -1, 1, 0, 0, [node, &initial_left, &initial_right](int, int xoff_left, int yoff_left, bool, int, int) {
        traverse_inner(node->right, 1, 1, 0, 0, [xoff_left, yoff_left, &initial_left, &initial_right](int, int xoff_right, int yoff_right, bool, int, int) {
            if (xoff_left == xoff_right && yoff_left == yoff_right) {
                initial_left -= 1;
                initial_right += 1;
            }
        });
    });
}

template <typename F>
void traverse_inner_adjust(Node * node, int xoff, int yoff, int parent_xoff, int parent_yoff, F inspect) {
    inspect(node->data, xoff, yoff, true, parent_xoff, parent_yoff);
    int initial_left = xoff - 1;
    int initial_right = xoff + 1;
    traverse_overlap_adjust(node, initial_left, initial_right);
    if (node->left) {
        traverse_inner_adjust(node->left, initial_left, yoff + 1, xoff, yoff, inspect);
    }
    if (node->right) {
        traverse_inner_adjust(node->right, initial_right, yoff + 1, xoff, yoff, inspect);
    }
}

template <typename F>
void traverse(Node * root, F inspect) {
    // Inspect root
    inspect(root->data, 0, 0, false, 0, 0);
    // Adjust child horizontal offsets if there are any nodes with the same position
    int initial_left = -1;
    int initial_right = 1;
    // Finally traverse with the adjusted positions
    if (root->left) {
        traverse_inner_adjust(root->left, initial_left, 1, 0, 0, inspect);
    }
    if (root->right) {
        traverse_inner_adjust(root->right, initial_right, 1, 0, 0, inspect);
    }
}

void sfml_vis(Node * root) {
    sf::RenderWindow window(sf::VideoMode(900, 900), "Visual BST");
    window.setVerticalSyncEnabled(true);
    sf::Font font;
    if (!font.loadFromFile("ScarletJosephine.ttf")) {
        std::cout << "Failed to load font" << std::endl;
        std::abort();
    }
    int circle_radius = 50;
    sf::CircleShape nodeCircle;
    nodeCircle.setOutlineThickness(2);
    nodeCircle.setOrigin(circle_radius / 2, circle_radius / 2);
    nodeCircle.setRadius(circle_radius);
    sf::Text nodeText("", font, 24);
    sf::View view(sf::Vector2f(0, 230), sf::Vector2f(window.getSize().x, window.getSize().y));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        float speed = 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            view.move(-speed, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            view.move(speed, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            view.move(0, -speed);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            view.move(0, speed);
        }
        window.clear(sf::Color(21, 34, 56));
        window.setView(view);
        // Draw lines first
        traverse(root, [&window, circle_radius](int, int xoff, int yoff, bool has_parent, int parent_xoff, int parent_yoff) {
            int off_x = xoff * circle_radius * 2;
            int off_y = yoff * circle_radius * 2;
            if (has_parent) {
                sf::Vertex line[] =
                {
                    sf::Vertex(sf::Vector2f(circle_radius / 2 + parent_xoff * circle_radius * 2, circle_radius / 2 + parent_yoff * circle_radius * 2), sf::Color::White),
                    sf::Vertex(sf::Vector2f(circle_radius / 2 + off_x, circle_radius / 2 + off_y), sf::Color::White)
                };
                window.draw(line, 2, sf::Lines);
            }
        });
        // Then the node circles on top
        traverse(root, [&nodeCircle, &nodeText, &window, circle_radius](int value, int xoff, int yoff, bool, int, int) {
            sf::Color color = sf::Color(0, 100, 0);
            sf::Color complement = sf::Color(248, 149, 100);
            int off_x = xoff * circle_radius * 2;
            int off_y = yoff * circle_radius * 2;
            nodeCircle.setPosition(off_x, off_y);
            nodeCircle.setFillColor(color);
            nodeCircle.setOutlineColor(complement);
            window.draw(nodeCircle);
            nodeText.setString(std::to_string(value));
            nodeText.setPosition(off_x, off_y);
            nodeText.setFillColor(complement);
            window.draw(nodeText);
        });
        window.display();
    }
}

int main()
{
    Node *root = insertBST(NULL, 20);
    cout << endl;
    int option,v;

    do {
        cout << "What operation do you want to perform? " <<
             " Select Option number. Enter 0 to exit." << endl;
        cout << "1. Insert Node" << endl;
        cout << "2. Search Node" << endl;
        cout << "3. Delete Node" << endl;
        cout << "4. Graphical Representation" << endl;
        cout << "5. Inorder Traversal" << endl;
        cout << "------------------------------------------------" << endl;
        cin >> option;
        switch (option) {
        case 0:
            break;
        case 1:
        {
            cout << "Insertion Selected" << endl;
            cout <<"Enter VALUE of TREE NODE to INSERT in BST: ";
            cin >> v;
            insertBST(root,v);
            break;
        }

        case 2:
        {
            cout << "Search Selected" << endl;
            cout << "Enter VALUE of TREE NODE to SEARCH in BST: ";
            cin >> v;
            break;
        }
        case 3:
        {
            cout << "Delete Selected" << endl;
            cout << "Enter VALUE of TREE NODE to Delete in BST: ";
            cin >> v;
            DeleteInBST(root,v);
            break;
        }
        case 4:
            sfml_vis(root);
            break;
        case 5:
        {
            cout << "Inorder Traversal Selected" << endl;
            inorder(root);
        }
        break;
        default:
            cout << "Enter Proper Option number " << endl;
        }

        cout << endl;
    } while (option != 0);
    return 0;
}
