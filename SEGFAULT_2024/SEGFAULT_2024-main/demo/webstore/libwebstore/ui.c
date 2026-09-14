#include <stdio.h>
#include <stdbool.h>

#include "logic.h"
#include "utils.h"
#include "ui.h"

#include "../../../src/refmem.h"

// Eventloop and all visual concerns.
static char print_menu()
{
    char *menu =
        "[A]dd merchandice\n"
        "[L]ist merchandice\n"
        "[D]elete merhandice\n"
        "[E]dit merchandice\n"
        "[S]how stock\n"
        "Re[P]lenish\n"
        "[C]reate cart\n"
        "[R]emove cart\n"
        "[+]Add to cart\n"
        "[-]Remove from cart\n"
        "[=]Calculate cost\n"
        "Check[O]ut\n"
        "[Q]uit\n";
    return ask_question_menu(menu, "AaLlDdEeSsPpCcRr+-=OoQq");
}

static void ask_merch(char **name, char **desc, int *price)
{
    // Used as key in hash table
    *name = ask_question_string("Name:");
    *desc = ask_question_string("Desc:");
    *price = ask_question_int("Price:");
}

static void event_loop(ioopm_wh_t *wh)
{
    char choice;
    bool run = true;
    while (run)
    {
        choice = print_menu();
        if (choice == 'A')
        {
            char *name;
            char *desc;
            int price;
            ask_merch(&name, &desc, &price);
            ioopm_add_merch(wh, name, desc, price);
        }
        else if (choice == 'L')
        {
            ioopm_print_wh_list(wh);
        }
        else if (choice == 'D')
        {
            char *name = ask_question_string("Which merch do you want to delete?"); // return strdup
            bool rewrite = ask_confirm("Are you sure?[Y/y]", 'y');
            if (rewrite)
            {
                ioopm_delete_merch(wh, name);
            }
            release(name);
        }
        else if (choice == 'E')
        {
            char *to_edit = ask_question_string("Merch to edit:");
            char *name;
            char *desc;
            int price;
            ask_merch(&name, &desc, &price);
            bool rewrite = ask_confirm("Save before exit?[Y/y]", 'y');
            if (rewrite)
            {
                ioopm_edit_merch(wh, to_edit, name, desc, price, rewrite);
            }
        }
        else if (choice == 'S')
        {
            char *name = ask_question_string("Show stock of: ");
            ioopm_show_stock(wh, name);
        }
        else if (choice == 'P')
        {
            char *shelf = ask_question_shelf("Shelf: ");
            char *name = ask_question_string("Merch: ");
            int quantity = ask_question_int("Qty: ");
            ioopm_replenish_merch(wh, shelf, name, quantity);
        }
        else if (choice == 'C')
        {
            int cart_id = ioopm_create_cart(wh);
            printf("Your cart id is %d: \n", cart_id);
        }
        else if (choice == 'R')
        {
            int cart_id = ask_question_int("Cart id: ");
            bool rewrite = ask_confirm("Are you sure?[Y/y]", 'y');
            if (rewrite)
            {
                ioopm_remove_cart(wh, cart_id);
            }
        }
        else if (choice == '+')
        {
            int cart_id = ask_question_int("Cart id: ");
            char *merch_name = ask_question_string("Merch name: ");
            int quantity = ask_question_int("Quantity: ");
            ioopm_add_to_cart(wh, cart_id, merch_name, quantity);
        }
        else if (choice == '-')
        {
            int cart_id = ask_question_int("Cart id: ");
            char *merch_name = ask_question_string("Merch name: ");
            int quantity = ask_question_int("Quantity to remove: ");
            ioopm_remove_from_cart(wh, cart_id, merch_name, quantity);
        }
        else if (choice == '=')
        {
            int cart_id = ask_question_int("Cart id: ");
            int total_cost = ioopm_calculate_cost(wh, cart_id);
            printf("Cost for cart = %d\n", total_cost);
        }
        else if (choice == 'O')
        {
            int cart_id = ask_question_int("Cart id: ");
            ioopm_checkout(wh, cart_id);
        }
        else if (choice == 'Q')
        {
            if (ask_confirm("Do you really want to quit?[Y/y]", 'y'))
            {
                run = false;
            }
        }
    }
}

// The main entry point for the ui
void start_ui(void) {
    ioopm_wh_t *wh = ioopm_wh_create();
    event_loop(wh);
    ioopm_wh_destroy(wh);
}
