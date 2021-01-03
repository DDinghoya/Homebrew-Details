#include <main.h>
#include <utils/blacklist.h>
#include <utils/favorites.h>
#include <utils/launching.h>
#include <utils/modules.h>
#include <utils/nacp_utils.h>
#include <utils/notes.h>
#include <utils/panels.h>
#include <utils/reboot_to_payload.h>
#include <utils/scanning.h>
#include <utils/settings.h>
#include <utils/update.h>
#include <utils/utilities.h>

#include <pages/info_page.hpp>
#include <pages/intro_page.hpp>
#include <pages/issue_page.hpp>
#include <pages/layout_select_page.hpp>
#include <pages/main_page.hpp>
#include <pages/updating_page.hpp>
//

#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <switch.h>

#include "switch/services/psm.h"

//
#include <sys/select.h>
//
#include <curl/curl.h>
#include <curl/easy.h>
//
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include <algorithm>
#include <array>
#include <borealis.hpp>
#include <cassert>
#include <chrono>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#ifndef APP_VERSION
#error APP_VERSION define missing
#endif

namespace fs = std::filesystem;

void purge_entry(app_entry* entry)
{
}

brls::ListItem* MainPage::make_app_entry(app_entry* entry, bool is_appstore)
{
    std::string label = entry->name;

    brls::ListItem* popupItem = new brls::ListItem(label, "", entry->full_path);
    popupItem->setValue("v" + entry->version);
    popupItem->setThumbnail(entry->icon, entry->icon_size);

    //popupItem->updateActionHint(brls::Key::Y, "Favorite");
    //popupItem->registerAction("Favorite", brls::Key::Y, [this, entry, popupItem]() {
    //    if (vector_contains(favorites, entry->full_path))
    //    {
    //        remove_favorite(entry->full_path);
    //        entry->favorite = false;

    //        print_debug("check: " + entry->name + " " + "v" + entry->version);
    //        print_debug(std::to_string(appsList->getViewsCount()));

    //        brls::List* list_type[3] = { appsList,
    //            localAppsList,
    //            storeAppsList };

    //        for (unsigned int list_iter = 0; list_iter < 3; list_iter++)
    //        {
    //            for (unsigned int i = 0; i < list_type[list_iter]->getViewsCount(); i++)
    //            {
    //                //print_debug(std::to_string(i));
    //                brls::ListItem* item = (brls::ListItem*)list_type[list_iter]->getChild(i);
    //                //print_debug("item+: " + item->getLabel() + ";" + item->getValue());
    //                //print_debug("item*: " + symbol_star() + "  " + entry->name + ";" + "v" + entry->version);

    //                if (symbol_star() + "  " + entry->name == item->getLabel() && "v" + entry->version == item->getValue())
    //                {
    //                    print_debug("Collapse: " + item->getLabel() + " " + item->getValue());

    //                    item->collapse(false);
    //                }
    //                else if (entry->name == item->getLabel() && "v" + entry->version == item->getValue())
    //                {
    //                    print_debug("Expand: " + item->getLabel() + " " + item->getValue());

    //                    item->expand(false);
    //                }
    //            }
    //        }

    //        //popupItem->setChecked(false);
    //    }
    //    else
    //    {
    //        add_favorite(entry->full_path);
    //        entry->favorite = true;

    //        print_debug("check: " + entry->name + " " + "v" + entry->version);
    //        print_debug(std::to_string(appsList->getViewsCount()));

    //        brls::List* list_type[3] = { appsList,
    //            localAppsList,
    //            storeAppsList };

    //        for (unsigned int list_iter = 0; list_iter < 3; list_iter++)
    //        {
    //            for (unsigned int i = 0; i < list_type[list_iter]->getViewsCount(); i++)
    //            {
    //                //print_debug(std::to_string(i));

    //                brls::ListItem* item = (brls::ListItem*)list_type[list_iter]->getChild(i);
    //                //print_debug("item+: " + item->getLabel() + ";" + item->getValue());
    //                //print_debug("item*: " + symbol_star() + "  " + entry->name + ";" + "v" + entry->version);

    //                if (symbol_star() + "  " + entry->name == item->getLabel() && "v" + entry->version == item->getValue())
    //                {
    //                    print_debug("Collapse: " + item->getLabel() + " " + item->getValue());

    //                    item->expand(false);
    //                }
    //                else if (entry->name == item->getLabel() && "v" + entry->version == item->getValue())
    //                {
    //                    print_debug("Expand: " + item->getLabel() + " " + item->getValue());

    //                    item->collapse(false);
    //                }
    //            }
    //        }
    //    }

    //    this->onCancel();

    //    appsList->invalidate();
    //    localAppsList->invalidate();
    //    storeAppsList->invalidate();

    //    return true;
    //});

    brls::Key key = brls::Key::A;
    if (get_setting(setting_control_scheme) == "0")
        key = brls::Key::X;
    else if (get_setting(setting_control_scheme) == "1")
        key = brls::Key::A;

    popupItem->updateActionHint(key, "Launch");
    popupItem->registerAction("Launch", key, [this, entry, popupItem]() {
        print_debug("launch app");
        unsigned int r = launch_nro(entry->full_path, "\"" + entry->full_path + "\"");
        print_debug("r: " + std::to_string(r));
        if (R_FAILED(r))
        {
            print_debug("Uh oh.");
        }
        else
        {
            brls::Application::quit();
        }

        return true;
    });

    key = brls::Key::X;
    if (get_setting(setting_control_scheme) == "0")
        key = brls::Key::A;
    else if (get_setting(setting_control_scheme) == "1")
        key = brls::Key::X;

    popupItem->updateActionHint(key, "Details");
    popupItem->registerAction("Details", key, [this, entry, popupItem, is_appstore]() {
        brls::TabFrame* appView = new brls::TabFrame();

        brls::List* manageList = new brls::List();
        manageList->addView(new brls::Header("File Management Actions", false));

        {
            brls::ListItem* launch_item = new brls::ListItem("Launch App");
            launch_item->getClickEvent()->subscribe([this, entry](brls::View* view) {
                print_debug("launch app");
                unsigned int r = launch_nro(entry->full_path, "\"" + entry->full_path + "\"");
                print_debug("r: " + std::to_string(r));
                if (R_FAILED(r))
                {
                    print_debug("Uh oh.");
                }
                else
                {
                    brls::Application::quit();
                }
            });
            manageList->addView(launch_item);
        }

        //
        if (!is_appstore)
        {
            brls::ListItem* move_item = new brls::ListItem("Move App");
            move_item->getClickEvent()->subscribe([entry, appView](brls::View* view) {
                std::string dest_string = get_keyboard_input(entry->full_path);

                if (to_lower(dest_string) == to_lower(entry->full_path))
                {
                    print_debug("Same path");
                    brls::Dialog* dialog                       = new brls::Dialog("Source and destination are the same.");
                    brls::GenericEvent::Callback closeCallback = [dialog](brls::View* view) {
                        dialog->close();
                    };
                    dialog->addButton("Dismiss", closeCallback);
                    dialog->setCancelable(true);
                    dialog->open();
                }
                else if (dest_string.length() <= 4 || (dest_string).substr(dest_string.length() - 4) != ".nro")
                {
                    print_debug("Isn't an nro");
                    brls::Dialog* dialog                       = new brls::Dialog("Your destination,\n'" + dest_string + "'\ndoesn't end with '.nro'.");
                    brls::GenericEvent::Callback closeCallback = [dialog](brls::View* view) {
                        dialog->close();
                    };
                    dialog->addButton("Dismiss", closeCallback);
                    dialog->setCancelable(true);
                    dialog->open();
                }
                else if (fs::exists(dest_string))
                {
                    print_debug("File already exists.");
                    brls::Dialog* dialog                       = new brls::Dialog("Your destination,\n'" + dest_string + "'\nalready exists.");
                    brls::GenericEvent::Callback closeCallback = [dialog](brls::View* view) {
                        dialog->close();
                    };
                    dialog->addButton("Dismiss", closeCallback);
                    dialog->setCancelable(true);
                    dialog->open();
                }
                else
                {
                    std::size_t found = dest_string.find_last_of("/");
                    if (found != std::string::npos)
                    {
                        create_directories(dest_string.substr(0, found));
                    }

                    brls::Dialog* confirm_dialog             = new brls::Dialog("Are you sure you want to move the following file? This action cannot be undone.\n\n" + entry->full_path + "\n" + symbol_downarrow() + "\n" + dest_string);
                    brls::GenericEvent::Callback yesCallback = [confirm_dialog, entry, appView, dest_string](brls::View* view) {
                        if (rename(entry->full_path.c_str(), dest_string.c_str()) != 0)
                            brls::Application::notify("Issue moving file");
                        else
                        {
                            brls::Application::notify("File successfully moved");
                            purge_entry(entry);
                        }

                        confirm_dialog->close();
                    };
                    brls::GenericEvent::Callback noCallback = [confirm_dialog](brls::View* view) {
                        confirm_dialog->close();
                    };
                    confirm_dialog->addButton("!!  [Yes]  !!", yesCallback);
                    confirm_dialog->addButton("No", noCallback);
                    confirm_dialog->setCancelable(false);
                    confirm_dialog->open();
                }
            });
            manageList->addView(move_item);
        }
        //
        {
            brls::ListItem* copy_item = new brls::ListItem("Copy App");
            copy_item->getClickEvent()->subscribe([entry, appView](brls::View* view) {
                std::string dest_string = get_keyboard_input(entry->full_path);

                if (to_lower(dest_string) == to_lower(entry->full_path))
                {
                    print_debug("Same path");
                    brls::Dialog* dialog                       = new brls::Dialog("Source and destination are the same.");
                    brls::GenericEvent::Callback closeCallback = [dialog](brls::View* view) {
                        dialog->close();
                    };
                    dialog->addButton("Dismiss", closeCallback);
                    dialog->setCancelable(true);
                    dialog->open();
                }
                else if (dest_string.length() <= 4 || (dest_string).substr(dest_string.length() - 4) != ".nro")
                {
                    print_debug("Isn't an nro");
                    brls::Dialog* dialog                       = new brls::Dialog("Your destination,\n'" + dest_string + "'\ndoesn't end with '.nro'.");
                    brls::GenericEvent::Callback closeCallback = [dialog](brls::View* view) {
                        dialog->close();
                    };
                    dialog->addButton("Dismiss", closeCallback);
                    dialog->setCancelable(true);
                    dialog->open();
                }
                else if (fs::exists(dest_string))
                {
                    print_debug("File already exists.");
                    brls::Dialog* dialog                       = new brls::Dialog("Your destination,\n'" + dest_string + "'\nalready exists.");
                    brls::GenericEvent::Callback closeCallback = [dialog](brls::View* view) {
                        dialog->close();
                    };
                    dialog->addButton("Dismiss", closeCallback);
                    dialog->setCancelable(true);
                    dialog->open();
                }
                else
                {
                    std::size_t found = dest_string.find_last_of("/");
                    if (found != std::string::npos)
                    {
                        create_directories(dest_string.substr(0, found));
                    }

                    brls::Dialog* confirm_dialog             = new brls::Dialog("Are you sure you want to copy to the following file? This action cannot be undone.\n\n" + entry->full_path + "\n" + symbol_downarrow() + "\n" + dest_string);
                    brls::GenericEvent::Callback yesCallback = [confirm_dialog, entry, appView, dest_string](brls::View* view) {
                        if (copy_file(entry->full_path.c_str(), dest_string.c_str()))
                        {
                            brls::Application::notify("File successfully copied");
                            purge_entry(entry);
                        }
                        else
                        {
                            brls::Application::notify("Issue copying file");
                        }

                        confirm_dialog->close();
                    };
                    brls::GenericEvent::Callback noCallback = [confirm_dialog](brls::View* view) {
                        confirm_dialog->close();
                    };
                    confirm_dialog->addButton("!!  [Yes]  !!", yesCallback);
                    confirm_dialog->addButton("No", noCallback);
                    confirm_dialog->setCancelable(false);
                    confirm_dialog->open();
                }
            });
            manageList->addView(copy_item);
        }
        //
        {
            brls::ListItem* delete_item = new brls::ListItem("Delete App");
            delete_item->getClickEvent()->subscribe([entry, appView](brls::View* view) {
                brls::Dialog* dialog                     = new brls::Dialog("Are you sure you want to delete the following file? This action cannot be undone.\n\n" + entry->full_path);
                brls::GenericEvent::Callback yesCallback = [dialog, entry, appView](brls::View* view) {
                    if (remove(entry->full_path.c_str()) != 0)
                        brls::Application::notify("Issue removing file");
                    else
                    {
                        std::string _folder = folder_of_file(entry->full_path);
                        print_debug("_folder " + _folder);

                        const char* basePath = _folder.c_str();

                        struct dirent* dp;
                        DIR* dir = opendir(basePath);

                        int num_files = 0;
                        while ((dp = readdir(dir)) != NULL)
                        {
                            print_debug("content: " + std::string(basePath) + std::string(dp->d_name));
                            num_files += 1;
                        }

                        closedir(dir);

                        print_debug("num things: " + std::to_string(num_files));

                        if (num_files == 0)
                        {
                            int res = rmdir(_folder.c_str());
                            print_debug("res " + std::to_string(res));

                            print_debug("No other files, removing " + _folder);
                        }
                        else
                            print_debug("Other files, not removing " + _folder);

                        brls::Application::notify("File successfully deleted");
                        purge_entry(entry);
                    }

                    dialog->close();
                };
                brls::GenericEvent::Callback noCallback = [dialog](brls::View* view) {
                    dialog->close();
                };
                dialog->addButton("!!  [Yes]  !!", yesCallback);
                dialog->addButton("No", noCallback);
                dialog->setCancelable(true);
                dialog->open();
            });
            manageList->addView(delete_item);
        }

        appView->addTab("Manage", manageList);

        int entry_width = 21;

        brls::List* appInfoList = new brls::List();
        appInfoList->addView(new brls::Header(".NRO File Info", false));
        add_list_entry("Name", entry->name, "", appInfoList, entry_width);
        add_list_entry("Filename", entry->file_name, "Full Path:\n\n" + entry->full_path, appInfoList, entry_width);
        add_list_entry("Author", entry->author, "", appInfoList, entry_width);
        add_list_entry("Version", entry->version, "", appInfoList, entry_width);
        add_list_entry("Size", to_megabytes(entry->size) + " MB", "Exact Size:\n\n" + std::to_string(entry->size) + " bytes", appInfoList, entry_width);
        add_list_entry("Icon Size", std::to_string(entry->icon_size), "", appInfoList, entry_width);
        appView->addTab("File Info", appInfoList);

        brls::List* appStoreInfoList = new brls::List();
        appStoreInfoList->addView(new brls::Header("App Store Info", false));

        add_list_entry("From Appstore", (entry->from_appstore ? "Yes" : "No"), "", appStoreInfoList, entry_width);
        add_list_entry("URL", entry->url, "", appStoreInfoList, entry_width);
        add_list_entry("Category", entry->category, "", appStoreInfoList, entry_width);
        add_list_entry("License", entry->license, "", appStoreInfoList, entry_width);
        add_list_entry("Description", entry->description, "", appStoreInfoList, entry_width);
        add_list_entry("Summary", entry->summary, "", appStoreInfoList, entry_width);
        add_list_entry("Changelog", entry->changelog, "", appStoreInfoList, entry_width);

        appView->addTab("App Store Info", appStoreInfoList);

        {
            brls::List* notesList      = new brls::List();
            brls::ListItem* notes_item = new brls::ListItem("Edit Notes");
            brls::Label* desc          = new brls::Label(brls::LabelStyle::DESCRIPTION, notes_get_value(entry->file_name), true);
            brls::Label* note_header   = new brls::Label(brls::LabelStyle::REGULAR, "Notes:", false);
            note_header->setVerticalAlign(NVGalign::NVG_ALIGN_TOP);

            if (notes_get_value(entry->file_name).empty())
                note_header->collapse(true);
            else
                note_header->expand(true);

            notes_item->getClickEvent()->subscribe([entry, appView, desc, note_header](brls::View* view) {
                std::string dest_string = get_keyboard_input(notes_get_value(entry->file_name));
                notes_set_value(entry->file_name, dest_string);
                desc->setText(dest_string);

                if (dest_string.empty())
                    note_header->collapse(true);
                else
                    note_header->expand(true);
            });

            notesList->addView(notes_item);
            notesList->addView(note_header);
            notesList->addView(desc);
            appView->addTab("Notes", notesList);
        }

        brls::PopupFrame::open(entry->name, entry->icon, entry->icon_size, appView, "Author: " + entry->author, "Version: " + entry->version);

        return true;
    });

    return popupItem;
}

//void MainPage::build_main_tabs()
//{
//    std::vector<brls::ListItem*> to_collapse;
//
//    appsList      = new brls::List();
//    storeAppsList = new brls::List();
//    localAppsList = new brls::List();
//
//    for (unsigned int i = 0; i < local_apps.size(); i++)
//    {
//        app_entry* current  = &local_apps.at(i);
//        std::string old_str = current->name;
//        current->name       = symbol_star() + "  " + current->name;
//
//        brls::ListItem* item_apps = make_app_entry(current, false);
//
//        //if (!current->favorite)
//        //    item_apps->collapse();
//
//        appsList->addView(item_apps);
//
//        if (!current->favorite)
//            to_collapse.push_back(item_apps);
//
//        if (current->from_appstore)
//        {
//            brls::ListItem* item_store = make_app_entry(current, true);
//            storeAppsList->addView(item_store);
//
//            if (!current->favorite)
//                to_collapse.push_back(item_store);
//        }
//        else
//        {
//            brls::ListItem* item_local = make_app_entry(current, false);
//            localAppsList->addView(item_local);
//
//            if (!current->favorite)
//                to_collapse.push_back(item_local);
//        }
//
//        current->name = old_str;
//    }
//
//    for (unsigned int i = 0; i < local_apps.size(); i++)
//    {
//        app_entry* current = &local_apps.at(i);
//
//        brls::ListItem* item_apps = make_app_entry(current, false);
//
//        //if (current->favorite)
//        //    item_apps->collapse();
//
//        appsList->addView(item_apps);
//
//        if (current->favorite)
//            to_collapse.push_back(item_apps);
//
//        if (current->from_appstore)
//        {
//            brls::ListItem* item_store = make_app_entry(current, true);
//            storeAppsList->addView(item_store);
//
//            if (current->favorite)
//                to_collapse.push_back(item_store);
//        }
//        else
//        {
//            brls::ListItem* item_local = make_app_entry(current, false);
//            localAppsList->addView(item_local);
//
//            if (current->favorite)
//                to_collapse.push_back(item_local);
//        }
//    }
//
//    if (!local_apps.empty() && !store_apps.empty())
//    {
//        this->addTab(pad_string_with_spaces("All Apps", store_apps.size() + local_apps.size(), 20).c_str(), appsList);
//        this->addSeparator();
//    }
//
//    if (!store_apps.empty())
//        this->addTab(pad_string_with_spaces("App Store Apps", store_apps.size(), 9).c_str(), storeAppsList);
//    if (!local_apps.empty())
//        this->addTab(pad_string_with_spaces("Local Apps", local_apps.size(), 16).c_str(), localAppsList);
//
//    while (!to_collapse.empty())
//    {
//        to_collapse.at(0)->collapse(false);
//        to_collapse.erase(to_collapse.begin());
//    }
//}

//void hide_by_fav(brls::List* list)
//{
//    while (!to_collapse.empty())
//    {
//        to_collapse.at(0)->collapse(false);
//        to_collapse.erase(to_collapse.begin());
//    }
//}

MainPage::MainPage()
{
    std::string title = "Homebrew Details v" + get_setting(setting_local_version);
    if (get_setting_true(setting_debug))
        title += " [Debug Mode]";

    this->setTitle(title.c_str());
    this->setIcon(get_resource_path("icon.png"));
    print_debug("init rootframe");

    //build_main_tabs();

    //hide_by_fav(appsList);

    //rootFrame->addSeparator();
    //rootFrame->addTab("Applications", new brls::Rectangle(nvgRGB(120, 120, 120)));
    //rootFrame->addTab("Emulators", new brls::Rectangle(nvgRGB(120, 120, 120)));
    //rootFrame->addTab("Games", new brls::Rectangle(nvgRGB(120, 120, 120)));
    //rootFrame->addTab("Tools", new brls::Rectangle(nvgRGB(120, 120, 120)));
    //rootFrame->addTab("Misc.", new brls::Rectangle(nvgRGB(120, 120, 120)));

    //this->addTab("Read: "+std::to_string(batteryCharge), new brls::Rectangle(nvgRGB(120, 120, 120)));

    print_debug("Check for updates.");
    if (get_online_version_available())
    {
        brls::Application::notify("Update Available!\nPress L for more info.");

        this->registerAction("Update Info", brls::Key::L, [&]() {
            show_update_panel();
            return true;
        });
    }

    print_debug("Toolbox.");
    {
        this->addSeparator();

        brls::List* tools_list = new brls::List();

        tools_list->addView(new brls::Header("Actionables"));
        brls::ListItem* rtp_item = new brls::ListItem("Reboot to Payload");
        rtp_item->setValue("atmosphere/reboot_payload.bin");
        rtp_item->getClickEvent()->subscribe([](brls::View* view) {
            print_debug("reboot_to_payload");
            int result = reboot_to_payload();
            if (result == -1)
                brls::Application::notify("Problem initializing spl");
            else if (result == -2)
                brls::Application::notify("Failed to open atmosphere/ reboot_payload.bin!");
        });
        tools_list->addView(rtp_item);

        tools_list->addView(new brls::Header("Information"));

        brls::ListItem* nsp_item = new brls::ListItem("How to Install to Home Menu...");
        nsp_item->getClickEvent()->subscribe([](brls::View* view) {
            brls::TabFrame* appView = new brls::TabFrame();
            appView->sidebar->setWidth(1000);
            appView->setHeight(400);
            appView->sidebar->addView(new brls::Label(brls::LabelStyle::REGULAR, "\nUsing your favorite nsp installer, install the forwarder that is currently in:\n- sdmc:/config/homebrew_details/forwarder/HomebrewDetails_MultiForwarder.nsp\n\nIt will launch this application from any of the following locations:\n- sdmc:/switch/homebrew_details.nro\n- sdmc:/switch/homebrew_details/homebrew_details.nro\n- sdmc:/switch/homebrew-details/homebrew_details.nro", true));
            appView->setIcon(get_resource_path("arrows.png"));
            brls::PopupFrame::open("How to Install to Home Menu", appView, "", "");
        });
        tools_list->addView(nsp_item);

        this->addTab("Toolbox", tools_list);
    }

    print_debug("Settings.");
    {
        brls::List* settings_list = new brls::List();
        settings_list->addView(new brls::Header("Scan Settings"));

        brls::ListItem* autoscan_switch = new brls::ListItem("Autoscan", "", "Begin scanning as soon as the app is launched.");
        autoscan_switch->setChecked((get_setting_true(setting_autoscan)));
        autoscan_switch->updateActionHint(brls::Key::A, "Toggle");
        autoscan_switch->getClickEvent()->subscribe([autoscan_switch](brls::View* view) {
            if (get_setting(setting_autoscan) == "true")
            {
                set_setting(setting_autoscan, "false");
                autoscan_switch->setChecked(false);
            }
            else
            {
                set_setting(setting_autoscan, "true");
                autoscan_switch->setChecked(true);
            }
        });
        settings_list->addView(autoscan_switch);

        brls::ListItem* item_scan_switch = new brls::ListItem("Scan /switch/");
        item_scan_switch->setChecked(true);
        brls::ListItem* item_scan_switch_subs = new brls::ListItem("Scan /switch/'s subfolders");
        item_scan_switch_subs->setChecked((get_setting_true(setting_search_subfolders)));
        item_scan_switch_subs->updateActionHint(brls::Key::A, "Toggle");
        item_scan_switch_subs->getClickEvent()->subscribe([item_scan_switch_subs](brls::View* view) {
            if (get_setting(setting_search_subfolders) == "true")
            {
                set_setting(setting_search_subfolders, "false");
                item_scan_switch_subs->setChecked(false);
            }
            else
            {
                set_setting(setting_search_subfolders, "true");
                item_scan_switch_subs->setChecked(true);
            }

            set_setting(setting_scan_settings_changed, "true");
        });

        brls::ListItem* item_scan_root = new brls::ListItem("Scan / (not subfolders)");
        item_scan_root->setChecked((get_setting_true(setting_search_root)));
        item_scan_root->updateActionHint(brls::Key::A, "Toggle");
        item_scan_root->getClickEvent()->subscribe([item_scan_root](brls::View* view) {
            if (get_setting(setting_search_root) == "true")
            {
                set_setting(setting_search_root, "false");
                item_scan_root->setChecked(false);
            }
            else
            {
                set_setting(setting_search_root, "true");
                item_scan_root->setChecked(true);
            }

            set_setting(setting_scan_settings_changed, "true");
        });

        brls::SelectListItem* layerSelectItem = new brls::SelectListItem("Scan Range", { "Scan Whole SD Card (Slow!)", "Only scan some folders" });

        layerSelectItem->getValueSelectedEvent()->subscribe([item_scan_switch, item_scan_switch_subs, item_scan_root](size_t selection) {
            switch (selection)
            {
                case 1:
                    set_setting(setting_scan_full_card, "false");
                    item_scan_switch->expand(true);
                    item_scan_switch_subs->expand(true);
                    item_scan_root->expand(true);
                    break;
                case 0:
                    set_setting(setting_scan_full_card, "true");
                    item_scan_switch->collapse(true);
                    item_scan_switch_subs->collapse(true);
                    item_scan_root->collapse(true);
                    break;
            }

            set_setting(setting_scan_settings_changed, "true");
        });
        settings_list->addView(layerSelectItem);
        settings_list->addView(item_scan_switch);
        settings_list->addView(item_scan_switch_subs);
        settings_list->addView(item_scan_root);

        if (get_setting(setting_scan_full_card) == "false")
        {
            layerSelectItem->setSelectedValue(1);
            item_scan_switch->expand(true);
            item_scan_switch_subs->expand(true);
            item_scan_root->expand(true);
        }
        else
        {
            layerSelectItem->setSelectedValue(0);
            item_scan_switch->collapse(true);
            item_scan_switch_subs->collapse(true);
            item_scan_root->collapse(true);
        }

        //

        settings_list->addView(new brls::Header("Blacklist Settings"));
        bl_vec.clear();
        bl_vec.push_back("Add new entry...");
        for (unsigned int ii = 0; ii < blacklist.size(); ii++)
            bl_vec.push_back(blacklist.at(ii));

        brls::ListItem* bl_edit_item = new brls::ListItem("Edit Blacklist");
        bl_edit_item->registerAction("Edit", brls::Key::A, [&]() {
            brls::TabFrame* appView = new brls::TabFrame();
            appView->sidebar->setWidth(1000);
            appView->addTab("Add new folder", nullptr);
            appView->sidebar->getChild(0)->registerAction("OK", brls::Key::A, [appView]() {
                std::string input = get_keyboard_input("sdmc:/switch/ignore_this");
                if (!input.empty())
                    add_blacklist(input);

                return appView->onCancel();
            });

            if (!blacklist.empty())
                appView->addSeparator();

            for (unsigned int iii = 0; iii < blacklist.size(); iii++)
            {
                const char* item_name = blacklist.at(iii).c_str();
                appView->addTab(item_name, nullptr);
                appView->sidebar->getChild(iii + 2)->registerAction("Delete Entry", brls::Key::X, [appView, item_name]() {
                    remove_blacklist(item_name);
                    return appView->onCancel();
                });
                appView->sidebar->getChild(iii + 2)->registerAction("Edit Entry", brls::Key::A, [appView, item_name]() {
                    std::string prev_string = item_name;
                    remove_blacklist(prev_string);

                    std::string input = get_keyboard_input(prev_string);
                    if (!input.empty())
                        add_blacklist(input);

                    return appView->onCancel();
                });
            }

            brls::PopupFrame::open("Blacklisted Folders", appView, "", "");
            return true;
        });
        settings_list->addView(bl_edit_item);

        //

        settings_list->addView(new brls::Header("Control Settings"));

        brls::SelectListItem* controlSelectItem = new brls::SelectListItem("Control Settings", { "A: Details; X: Launch", "A: Launch; X: Details" }, std::stoi(get_setting(setting_control_scheme)), "Takes full effect on next launch.");
        controlSelectItem->getValueSelectedEvent()->subscribe([](size_t selection) {
            set_setting(setting_control_scheme, std::to_string(selection));
        });
        settings_list->addView(controlSelectItem);

        //

        settings_list->addView(new brls::Header("App Settings"));

        brls::SelectListItem* exitToItem = new brls::SelectListItem("Exit To", { "sdmc:/hbmenu.nro", get_setting(setting_nro_path) });
        exitToItem->setValue(get_setting(setting_exit_to));
        exitToItem->getValueSelectedEvent()->subscribe([](size_t selection) {
            if (selection == 0)
                set_setting(setting_exit_to, "sdmc:/hbmenu.nro");
            else if (selection == 1)
                set_setting(setting_exit_to, get_setting(setting_nro_path));

            std::string target = get_setting(setting_exit_to);
            envSetNextLoad(target.c_str(), (std::string("\"") + target + "\"").c_str());
        });
        settings_list->addView(exitToItem);

        //
        print_debug("Misc.");
        settings_list->addView(new brls::Header("Misc. Settings"));

        brls::ListItem* debug_switch = new brls::ListItem("Debug Mode", "Takes full effect on next launch.");
        debug_switch->setChecked(get_setting_true(setting_debug));
        debug_switch->updateActionHint(brls::Key::A, "Toggle");
        debug_switch->getClickEvent()->subscribe([debug_switch](brls::View* view) {
            if (get_setting(setting_debug) == "true")
            {
                set_setting(setting_debug, "false");
                debug_switch->setChecked(false);
            }
            else
            {
                set_setting(setting_debug, "true");
                debug_switch->setChecked(true);
            }
        });
        settings_list->addView(debug_switch);
        //

        this->addTab("Settings", settings_list);
    }

    print_debug("Debug Menu.");
    if (get_setting_true(setting_debug))
    {
        this->addSeparator();
        brls::List* debug_list = new brls::List();
        debug_list->addView(new brls::Header("Super Secret Dev Menu Unlocked!", false));

        int entry_width = 21;

        std::uint32_t batteryCharge = 0;
        psmGetBatteryChargePercentage(&batteryCharge);
        add_list_entry("Battery Percent", std::to_string(batteryCharge) + "%", "", debug_list, entry_width);

        ChargerType chargerType;
        std::string chargerTypes[3] = { std::string("None"), std::string("Charging"), std::string("USB") };
        psmGetChargerType(&chargerType);
        std::string chargeStatus = "Error";
        if ((int)chargerType >= 0 && (int)chargerType < 3)
            chargeStatus = chargerTypes[chargerType];

        add_list_entry("Charging Status", chargeStatus, "", debug_list, entry_width);
        add_list_entry("Local Version", std::string("v") + get_setting(setting_local_version), "", debug_list, entry_width);
        add_list_entry("Online Version", std::string("v") + get_online_version_number(), "", debug_list, entry_width);
        //add_list_entry("Number of App Store Apps", std::to_string(store_apps.size()), "", debug_list, entry_width);
        //add_list_entry("Number of Local Apps", std::to_string(local_apps.size()), "", debug_list, entry_width);
        add_list_entry("Free Space", get_free_space(), "", debug_list, entry_width);

        brls::ListItem* rtp_item = new brls::ListItem("Reboot to Payload");
        rtp_item->getClickEvent()->subscribe([](brls::View* view) {
            print_debug("reboot_to_payload");
            reboot_to_payload();
        });
        debug_list->addView(rtp_item);

        brls::ListItem* keyboard_item = new brls::ListItem("Keyboard Test");
        keyboard_item->setValue("Test Text");
        keyboard_item->getClickEvent()->subscribe([keyboard_item](brls::View* view) {
            print_debug("Keyboard");
            std::string typed = get_keyboard_input(keyboard_item->getValue());
            keyboard_item->setValue(typed);
        });
        debug_list->addView(keyboard_item);

        add_list_entry("Number of Favorites", std::to_string(favorites.size()), "", debug_list, entry_width);
        add_list_entry("Number of Blacklisted Folders", std::to_string(blacklist.size()), "", debug_list, entry_width);

        brls::ListItem* test_grid_item = new brls::ListItem("Grid Layout Test");
        test_grid_item->getClickEvent()->subscribe([](brls::View* view) {
            brls::AppletFrame* fr = new brls::AppletFrame(true, true);
            fr->setIcon(get_resource_path("icon.png"));
            fr->setTitle("Choose your Menu Style");
            //fr->setFooterText("This can be changed later");
            fr->setContentView(new LayoutSelectPage());
            brls::Application::pushView(fr);
        });
        debug_list->addView(test_grid_item);

        brls::ListItem* test_list_item = new brls::ListItem("List Test");
        test_list_item->getClickEvent()->subscribe([this](brls::View* view) {
            brls::List* that_list = new brls::List();
            //for (unsigned int i = 0; i < local_apps.size(); i++)
            //{
            //    app_entry* current        = &local_apps.at(i);
            //    brls::ListItem* item_apps = make_app_entry(current, false);
            //    that_list->addView(item_apps);
            //}

            brls::AppletFrame* fr = new brls::AppletFrame(true, true);
            fr->setIcon(get_resource_path("icon.png"));
            fr->setTitle("Choose your Menu Style");
            fr->setFooterText("This can be changed later");
            fr->setContentView(appsList);
            brls::Application::pushView(fr);
        });
        debug_list->addView(test_list_item);

        this->addTab("Debug Menu", debug_list);
    }
}

MainPage::~MainPage()
{
}

void MainPage::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx)
{
    TabFrame::draw(vg, x, y, width, height, style, ctx);
    draw_status(this, x, y, width, height, style, ctx);
}