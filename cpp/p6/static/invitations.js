window.onload = function () {
  const container = document.getElementById("invitationsList");
  const params = new URLSearchParams(window.location.search);

  let content = "";
  const all = window.location.search.split("---");
  if (all.length === 0 || all[0] === "") {
    container.textContent = "No pending invitations.";
    return;
  }

  all.forEach(entry => {
    if (!entry.includes("id=")) return;

    const p = new URLSearchParams(entry);
    const id = p.get("id");
    const title = p.get("title");
    const date = p.get("date");
    const time = p.get("time");
    const desc = p.get("desc");

    content += `
      <div class="invitation">
        <h3>${title}</h3>
        <p><strong>Date:</strong> ${date} ${time}</p>
        <p><strong>Description:</strong> ${desc}</p>
        <form method="POST" action="/invitations">
          <input type="hidden" name="joinEventId" value="${id}">
          <button name="action" value="confirm" class="btn confirm">Confirm</button>
          <button name="action" value="reject" class="btn reject">Reject</button>
        </form>
      </div>
    `;
  });

  container.innerHTML = content;
};
