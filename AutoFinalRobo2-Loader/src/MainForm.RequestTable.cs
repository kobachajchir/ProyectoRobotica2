using System;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        readonly DataGridView requestTable = new DataGridView();
        Control CreateRequestTable()
        {
            requestTable.Dock = DockStyle.Fill; requestTable.ReadOnly = true; requestTable.AllowUserToAddRows = requestTable.AllowUserToDeleteRows = false;
            requestTable.RowHeadersVisible = false; requestTable.SelectionMode = DataGridViewSelectionMode.FullRowSelect; requestTable.MultiSelect = false;
            requestTable.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            foreach (string name in new[] { "Trabajo", "Acción", "COM", "Estado" }) requestTable.Columns.Add(name, name);
            requestTable.Columns.Add(new DataGridViewLinkColumn { HeaderText = "Solicitud", Text = "Ver detalles", UseColumnTextForLinkValue = true });
            foreach (DataGridViewColumn column in requestTable.Columns) column.SortMode = DataGridViewColumnSortMode.NotSortable;
            requestTable.CellContentClick += (s, e) => {
                if (e.RowIndex < 0 || e.ColumnIndex != 4) return;
                var job = (JobState)requestTable.Rows[e.RowIndex].Tag;
                using (var dialog = new Form { Text = "Solicitud MCP · " + job.Action, Size = new Size(850, 580), StartPosition = FormStartPosition.CenterParent, Font = Font })
                {
                    var text = new TextBox { ReadOnly = true, Multiline = true, ScrollBars = ScrollBars.Both, Dock = DockStyle.Fill, Text = pendingJob == job ? requestDetail.Text : job.Detail() };
                    EnableTextCopyFeedback(text); dialog.Controls.Add(text); ShowStyledDialog(dialog);
                }
            };
            requestDetail.TextChanged += (s, e) => RefreshRequestTable();
            timer.Tick += (s, e) => { if (mcpTab != null && mainTabs.SelectedTab == mcpTab) RefreshRequestTable(); };
            return requestTable;
        }
        void RefreshRequestTable()
        {
            if (requestTable.Columns.Count == 0) return;
            foreach (var job in jobs.Values.Where(j => j.Origin == "mcp").OrderByDescending(j => j.Created))
            {
                var row = requestTable.Rows.Cast<DataGridViewRow>().FirstOrDefault(r => Object.ReferenceEquals(r.Tag, job));
                if (row == null) { int index = requestTable.Rows.Add(job.Id.Substring(0, 8), job.Action, job.Port, job.Status); row = requestTable.Rows[index]; row.Tag = job; }
                if (!Object.Equals(row.Cells[3].Value, job.Status)) row.Cells[3].Value = job.Status;
            }
            foreach (var row in requestTable.Rows.Cast<DataGridViewRow>().Where(r => !jobs.ContainsKey(((JobState)r.Tag).Id)).ToArray()) requestTable.Rows.Remove(row);
        }
    }
}
