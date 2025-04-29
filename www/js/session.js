async function loadSessionData()
{
	const res = await fetch('/cgi_tester/session.cgi');
	const text = await res.text();

	console.log("[session.cgi output]", text);
	try
	{
		const session = JSON.parse(text);

		if (session.sessionid)
		{
			const el = document.getElementById('sessionid-display');
			if (el)
				el.textContent = session.sessionid;
		}
		if (session.username)
		{
			const el = document.getElementById('username-display');
			if (el)
				el.textContent = session.username;
		}
		if (session.status)
		{
			const el = document.getElementById('status-display');
			if (el)
				el.textContent = session.status;
		}
		if (session.email)
		{
			const el = document.getElementById('email-display');
			if (el)
				el.textContent = session.email;
		}
		if (session.password)
		{
			const el = document.getElementById('password-display');
			if (el)
				el.textContent = session.password;
		}
		if (session.theme)
		{
			const el = document.getElementById('theme-display');
			if (el)
				el.textContent = session.theme;
		}
		if (session.theme)
			document.body.classList.add(session.theme);
		window._SESSION = session;
	}
	catch (err)
	{
		console.error("Invalid JSON from session.cgi:", err.message);
	}
	
}

async function deleteSession()
{
	const sessionID = getCookieValue("sessionID");

	if (!sessionID)
	{
		alert("Aucune session à supprimer !");
		return;
	}

	const confirmDelete = confirm("Es-tu sûr de vouloir supprimer ta session ?");

	if (!confirmDelete)
		return;

	try
	{
		const formData = new URLSearchParams();
		formData.append("sessionID", sessionID);

		const response = await fetch("/cgi_tester/delete-session.cgi",
		{
			method: "POST",
			headers: { "Content-Type": "application/x-www-form-urlencoded" },
			body: formData.toString()
		});

		const text = await response.text();
		console.log("[delete-session.cgi output]", text);

		if (response.ok)
		{
			alert("Session supprimée !");
			document.cookie = "sessionID=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT"; // Supprime le cookie
			window.location.reload();
		}
		else
		{
			alert("Erreur lors de la suppression de la session : " + text);
		}
	}
	catch (err)
	{
		console.error("Erreur JS :", err);
		alert("Erreur de connexion au serveur.");
	}
}
