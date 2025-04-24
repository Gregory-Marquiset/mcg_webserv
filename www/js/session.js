async function loadSessionData()
{
	const res = await fetch('/cgi_tester/session.cgi');
	const text = await res.text();
	console.log("[session.cgi output]", text);
	try {
		const session = JSON.parse(text);
		if (session.sessionid) {
			const el = document.getElementById('sessionid-display');
			if (el) el.textContent = session.sessionid;
		}
		if (session.username) {
			const el = document.getElementById('username-display');
			if (el) el.textContent = session.username;
		}
		if (session.status) {
			const el = document.getElementById('status-display');
			if (el) el.textContent = session.status;
		}
		if (session.email) {
			const el = document.getElementById('email-display');
			if (el) el.textContent = session.email;
		}
		if (session.password) {
			const el = document.getElementById('password-display');
			if (el) el.textContent = session.password;
		}
		if (session.theme) {
			const el = document.getElementById('theme-display');
			if (el) el.textContent = session.theme;
		}
		if (session.theme) document.body.classList.add(session.theme);
		window._SESSION = session;
	} catch (err) {
		console.error("Invalid JSON from session.cgi:", err.message);
	}
	
}
