/*
  Requirements:

    sudo aptitude install libglib2.0-dev
    sudo aptitude install libgnome-keyring-dev

  If you are having trouble getting the libs to work, run

    pkg-config --cflags --libs gnome-keyring-1 glib-2.0

  to find out what needs to be sent to the compiler...


*/

#include <QtCore/QCoreApplication>
#include <QMap>
#include <QStringList>
#include <QDebug>

#include <glib.h>
#include <gnome-keyring.h>
#include <stdio.h>

QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

QString findKeyringItemPasswordForDescription(QString description)
{
    // Init
    GnomeKeyringAttributeList * attributes;
    GnomeKeyringResult result;
    GList * found_list;
    GList * i;
    GnomeKeyringFound * found;
    QString password;

    // Get all results
    attributes = g_array_new(FALSE, FALSE, sizeof (GnomeKeyringAttribute));
    result = gnome_keyring_find_items_sync(GNOME_KEYRING_ITEM_GENERIC_SECRET, attributes, &found_list);
    gnome_keyring_attribute_list_free(attributes);
    if (result != GNOME_KEYRING_RESULT_OK) return "";

    // Loop results, searching for a discription/dispalyname
    for (i = found_list; i != NULL; i = i->next) {
        // Extract the item info to compare description
        found = (GnomeKeyringFound *)i->data;
        GnomeKeyringItemInfo *info = gnome_keyring_item_info_new();
        gnome_keyring_item_get_info_sync (found->keyring,found->item_id,&info);
        QString displayName(gnome_keyring_item_info_get_display_name(info));
        gnome_keyring_item_info_free(info);

        // Matches?
        if(displayName == description) {
            password = QString(g_strdup(found->secret));
            break;
        }
    }

    // Cleanup and return
    gnome_keyring_found_list_free(found_list);
    return password;
}

QString findKeyringItemPasswordForAttributes(QMap<QString,QString> attributes)
{
    // Init
    GnomeKeyringAttributeList * gattributes;
    GnomeKeyringResult result;
    GList * found_list;
    GList * i;
    GnomeKeyringFound * found;
    QString password;

    // Compile an attribute list
    gattributes = g_array_new(FALSE, FALSE, sizeof (GnomeKeyringAttribute));
    QMapIterator<QString, QString> it(attributes);
    while (it.hasNext()) {
        it.next();
        gnome_keyring_attribute_list_append_string(gattributes,it.key().toLatin1(),it.value().toLatin1());
    }

    // Get results
    result = gnome_keyring_find_items_sync(GNOME_KEYRING_ITEM_GENERIC_SECRET, gattributes, &found_list);
    gnome_keyring_attribute_list_free(gattributes);
    if (result != GNOME_KEYRING_RESULT_OK) return "";

    // Loop results, taking the first match
    for (i = found_list; i != NULL; i = i->next) {
        // Extract the item info to compare description
        found = (GnomeKeyringFound *)i->data;
        password = QString(g_strdup(found->secret));
        break;
    }

    // Cleanup and return
    gnome_keyring_found_list_free(found_list);
    return password;
}


bool setKeyringItemPasswordForAttributes(QString title, QMap<QString,QString> attributes, QString password) {
    // Init
    GnomeKeyringAttributeList * gattributes;
    GnomeKeyringResult result;
    guint item_id;

    // Compile an attribute list
    gattributes = g_array_new(FALSE, FALSE, sizeof (GnomeKeyringAttribute));
    QMapIterator<QString, QString> it(attributes);
    while (it.hasNext()) {
        it.next();
        gnome_keyring_attribute_list_append_string(gattributes,it.key().toLatin1(),it.value().toLatin1());
    }

    // Create keyring item
    result = gnome_keyring_item_create_sync(NULL,GNOME_KEYRING_ITEM_GENERIC_SECRET,title.toLatin1(),gattributes,password.toLatin1(),TRUE,&item_id);
    gnome_keyring_attribute_list_free(gattributes);

    return (result == GNOME_KEYRING_RESULT_OK);
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Get the password from the gnome keyring and output it
    QString password = findKeyringItemPasswordForDescription(QCoreApplication::arguments().at(1));
    qStdOut() << password.toLatin1();
    qStdOut() << "\n";
    a.exit();
}
