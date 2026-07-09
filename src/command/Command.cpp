#include "command/Command.hpp"

/*
    Découpe une ligne IRC (sans le \r\n, déjà retiré par extractCommand) en Command.
    Forme : [:prefixe] COMMANDE middle1 middle2 ... [:trailing]
    Voir docs/PARSING.md pour les cas limites (trailing vide, ':' littéral, etc.).
*/
Command parseCommand(const std::string& str) {
    Command     cmd;
    size_t      i = 0;
    const size_t len = str.size();

    // 1. Préfixe optionnel : présent seulement si la ligne commence par ':'
    if (i < len && str[i] == ':') {
        size_t space = str.find(' ', i);
        if (space == std::string::npos) {
            // Ligne = juste un préfixe, rien d'autre -> commande vide
            cmd.prefix = str.substr(i + 1);
            return cmd;
        }
        cmd.prefix = str.substr(i + 1, space - (i + 1));
        i = space;
    }

    // Sauter les espaces avant la commande
    while (i < len && str[i] == ' ')
        ++i;

    // 2. Commande : jusqu'au prochain espace (ou fin de ligne)
    {
        size_t space = str.find(' ', i);
        if (space == std::string::npos) {
            cmd.command = str.substr(i);   // commande sans paramètre (ex: LIST)
            return cmd;
        }
        cmd.command = str.substr(i, space - i);
        i = space;
    }

    // 3. Paramètres
    while (i < len) {
        // Tolérance : sauter les espaces multiples entre middle params
        while (i < len && str[i] == ' ')
            ++i;
        if (i >= len)
            break;

        // Trailing : un ':' en DÉBUT de param -> tout le reste de la ligne
        // = un seul paramètre (espaces compris), puis on stoppe.
        if (str[i] == ':') {
            cmd.params.push_back(str.substr(i + 1));   // substr peut être "" (trailing vide)
            break;
        }

        // Middle param : jusqu'au prochain espace (ou fin de ligne)
        size_t space = str.find(' ', i);
        if (space == std::string::npos) {
            cmd.params.push_back(str.substr(i));
            break;
        }
        cmd.params.push_back(str.substr(i, space - i));
        i = space;
    }

    std::cout << "[CMD] Command parsed" << std::endl <<
        "Prefix: " << cmd.prefix << std::endl <<
        "Command: " << cmd.command << std::endl << 
        "Params: ";
    
    std::vector<std::string>::iterator it = cmd.params.begin();
    std::vector<std::string>::iterator ite = cmd.params.end();

    while (it != ite) {
        std::cout << *it;
        it++;
        if (it != ite)
            std::cout << ", ";
        else
            std::cout << "." << std::endl;
    }

    return cmd;
}
