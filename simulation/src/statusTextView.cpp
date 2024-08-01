#include "statusTextView.h"


GThread* intiThreadStatusTextView (GtkWidget * view,std::atomic<bool>* stop_thread){
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), FALSE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
    ThreadDataTextInfo thread_data = {buffer, stop_thread};
    return g_thread_new("update_thread", thread_func, &thread_data);
}

gpointer thread_func(gpointer data) {
    ThreadDataTextInfo* thread_data = (ThreadDataTextInfo*)data;
    GtkTextBuffer* buffer = thread_data->buffer;
    std::atomic<bool>* stop_thread = thread_data->stop_thread;
    while (!(*stop_thread)) {
        g_usleep(7000);

        std::string new_text = generateText();

        // Allouer et remplir la structure TextBufferData
        TextBufferData* tbd = (TextBufferData*)g_malloc(sizeof(TextBufferData));
        tbd->text = g_strdup(new_text.c_str());  // Dupliquer le texte en utilisant c_str()
        tbd->buffer = buffer;                    // Assigner le buffer

        // Utiliser g_idle_add pour exécuter update_text dans le thread principal GTK
        g_idle_add(update_text, tbd);
    }
    return NULL;
}

gboolean update_text(gpointer data) {
    TextBufferData* tbd = (TextBufferData*)data;
    gtk_text_buffer_set_text(tbd->buffer, tbd->text, -1);
    g_free(tbd->text);  // Libérer la mémoire allouée pour le texte
    g_free(tbd);        // Libérer la mémoire allouée pour la structure
    return FALSE;       // Retourner FALSE pour ne pas être appelé à nouveau
}

std::string generateText(void){
    std::string text;
    int16_t x;
    int16_t y;
    int16_t theta;
    get_coordinates(x,y,theta);
    text += "x: " + std::to_string(x) + '\n';
    text += "y: " + std::to_string(y) + '\n';
    text += "theta: " + std::to_string(theta) + '\n';
    return text;
}