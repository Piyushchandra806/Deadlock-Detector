#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAXP 5

int n, m;
int allocation[MAXP][MAXP], request[MAXP][MAXP];
int graph[MAXP][MAXP];
int visited[MAXP], stack_arr[MAXP];

GtkWidget *alloc_entries[MAXP][MAXP];
GtkWidget *req_entries[MAXP][MAXP];
GtkWidget *entry_p, *entry_r;
GtkWidget *drawing_area;

/* ================= CSS LOAD ================= */
void load_css()
{
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "style.css", NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
}

/* ================= DFS ================= */
int dfs(int v)
{
    visited[v] = 1;
    stack_arr[v] = 1;

    for (int i = 0; i < n; i++)
    {
        if (graph[v][i])
        {
            if (!visited[i] && dfs(i))
                return 1;
            else if (stack_arr[i])
                return 1;
        }
    }
    stack_arr[v] = 0;
    return 0;
}

/* ================= GRAPH DRAW ================= */
gboolean draw_graph(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    cairo_set_antialias(cr, CAIRO_ANTIALIAS_BEST);

    int cx[MAXP] = {250, 360, 250, 140, 250};
    int cy[MAXP] = {120, 240, 360, 240, 120};
    cairo_set_line_width(cr, 2.8);
    cairo_set_source_rgb(cr, 0.95, 0.27, 0.27); // bright red

    cairo_set_line_width(cr, 2.5);
    cairo_set_source_rgb(cr, 0, 0, 0); // black border

    cairo_set_source_rgb(cr, 0.22, 0.73, 0.97); // bright blue





    /* Draw edges */
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_set_line_width(cr, 2.5);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (graph[i][j])
            {
                double angle = atan2(cy[j] - cy[i], cx[j] - cx[i]);
                double sx = cx[i] + 22 * cos(angle);
                double sy = cy[i] + 22 * sin(angle);
                double ex = cx[j] - 22 * cos(angle);
                double ey = cy[j] - 22 * sin(angle);

                cairo_move_to(cr, sx, sy);
                cairo_line_to(cr, ex, ey);
                cairo_stroke(cr);

                /* Arrow */
                cairo_move_to(cr, ex, ey);
                cairo_line_to(cr, ex - 8 * cos(angle - 0.5),
                                   ey - 8 * sin(angle - 0.5));
                cairo_line_to(cr, ex - 8 * cos(angle + 0.5),
                                   ey - 8 * sin(angle + 0.5));
                cairo_fill(cr);
            }

    /* Draw nodes */
  /* Draw nodes (WHITE) */
/* Draw nodes */
for (int i = 0; i < n; i++)
{
    cairo_set_source_rgb(cr, 0.2, 0.6, 0.9); // BLUE node
    cairo_arc(cr, cx[i], cy[i], 22, 0, 2 * M_PI);
    cairo_fill_preserve(cr);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 2);
    cairo_stroke(cr);


    /* Process label */
    cairo_move_to(cr, cx[i] - 8, cy[i] + 5);
    char label[10];
    sprintf(label, "P%d", i);
    cairo_show_text(cr, label);
}

    return FALSE;
}

/* ================= DEADLOCK CHECK ================= */
void detect_deadlock(GtkWidget *widget, gpointer data)
{
    memset(graph, 0, sizeof(graph));
    memset(visited, 0, sizeof(visited));
    memset(stack_arr, 0, sizeof(stack_arr));

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
        {
            allocation[i][j] = atoi(gtk_entry_get_text(GTK_ENTRY(alloc_entries[i][j])));
            request[i][j] = atoi(gtk_entry_get_text(GTK_ENTRY(req_entries[i][j])));
        }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int r = 0; r < m; r++)
                if (request[i][r] > 0 && allocation[j][r] > 0 && i != j)
                    graph[i][j] = 1;

    int deadlock = 0;
    for (int i = 0; i < n; i++)
        if (!visited[i] && dfs(i))
            deadlock = 1;

    gtk_widget_queue_draw(drawing_area);

    /* POPUP */
    GtkWidget *dialog = gtk_dialog_new();
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 320, 160);

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *label = gtk_label_new(NULL);

    gtk_label_set_markup(GTK_LABEL(label),
        deadlock ?
        "<span font='18' foreground='red'><b>DEADLOCK DETECTED</b></span>" :
        "<span font='18' foreground='green'><b>NO DEADLOCK FOUND</b></span>"
    );

    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(content), label, TRUE, TRUE, 20);

    gtk_dialog_add_button(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_OK);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/* ================= MATRIX WINDOW ================= */
void open_matrix_window()
{
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Deadlock Analysis");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 500);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    GtkWidget *matrix_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_halign(matrix_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(matrix_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(main_box), matrix_box, TRUE, TRUE, 0);

    GtkWidget *frame_alloc = gtk_frame_new("Allocation Matrix");
    GtkWidget *frame_req = gtk_frame_new("Request Matrix");
    gtk_box_pack_start(GTK_BOX(matrix_box), frame_alloc, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(matrix_box), frame_req, FALSE, FALSE, 0);

    GtkWidget *grid_alloc = gtk_grid_new();
    GtkWidget *grid_req = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_alloc), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid_alloc), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid_req), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid_req), 8);

    gtk_container_add(GTK_CONTAINER(frame_alloc), grid_alloc);
    gtk_container_add(GTK_CONTAINER(frame_req), grid_req);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
        {
            alloc_entries[i][j] = gtk_entry_new();
            req_entries[i][j] = gtk_entry_new();
            gtk_entry_set_text(GTK_ENTRY(alloc_entries[i][j]), "0");
            gtk_entry_set_text(GTK_ENTRY(req_entries[i][j]), "0");
            gtk_grid_attach(GTK_GRID(grid_alloc), alloc_entries[i][j], j, i, 1, 1);
            gtk_grid_attach(GTK_GRID(grid_req), req_entries[i][j], j, i, 1, 1);
        }

    GtkWidget *btn = gtk_button_new_with_label("Detect Deadlock");
    gtk_box_pack_start(GTK_BOX(matrix_box), btn, FALSE, FALSE, 10);
    g_signal_connect(btn, "clicked", G_CALLBACK(detect_deadlock), NULL);

GtkWidget *graph_frame = gtk_frame_new("Wait-For Graph");
gtk_widget_set_size_request(graph_frame, 460, 460);
gtk_box_pack_start(GTK_BOX(main_box), graph_frame, TRUE, TRUE, 0);

drawing_area = gtk_drawing_area_new();
gtk_widget_set_size_request(drawing_area, 440, 420);
gtk_container_add(GTK_CONTAINER(graph_frame), drawing_area);

g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_graph), NULL);


    gtk_widget_show_all(window);
}

/* ================= NEXT ================= */
void next_clicked(GtkWidget *w, gpointer d)
{
    n = atoi(gtk_entry_get_text(GTK_ENTRY(entry_p)));
    m = atoi(gtk_entry_get_text(GTK_ENTRY(entry_r)));

    if (n > 0 && n <= MAXP && m > 0 && m <= MAXP)
        open_matrix_window();
}

/* ================= MAIN ================= */
int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    load_css();

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Deadlock Detection Tool");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 220);
GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
gtk_container_add(GTK_CONTAINER(window), vbox);

/* Title */
GtkWidget *title = gtk_label_new("Deadlock Detection Tool");
gtk_widget_set_name(title, "title");
gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 10);

/* Processes */
GtkWidget *p_label = gtk_label_new("Number of Processes");
entry_p = gtk_entry_new();
gtk_entry_set_placeholder_text(GTK_ENTRY(entry_p), "Enter processes (max 5)");

gtk_box_pack_start(GTK_BOX(vbox), p_label, FALSE, FALSE, 0);
gtk_box_pack_start(GTK_BOX(vbox), entry_p, FALSE, FALSE, 0);

/* Resources */
GtkWidget *r_label = gtk_label_new("Number of Resources");
entry_r = gtk_entry_new();
gtk_entry_set_placeholder_text(GTK_ENTRY(entry_r), "Enter resources (max 5)");

gtk_box_pack_start(GTK_BOX(vbox), r_label, FALSE, FALSE, 0);
gtk_box_pack_start(GTK_BOX(vbox), entry_r, FALSE, FALSE, 0);

/* Button */
GtkWidget *btn = gtk_button_new_with_label("Next");
gtk_box_pack_start(GTK_BOX(vbox), btn, FALSE, FALSE, 15);

g_signal_connect(btn, "clicked", G_CALLBACK(next_clicked), NULL);

    g_signal_connect(btn, "clicked", G_CALLBACK(next_clicked), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
