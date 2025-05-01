document.addEventListener("DOMContentLoaded", () =>
{
	const result = document.getElementById("result");
	const log = document.getElementById("log");

	// Formulaire HTTP général
	document.getElementById("httpForm").addEventListener("submit", async (e) =>
	{
		e.preventDefault();
		const url = document.getElementById("url").value;
		const method = document.getElementById("method").value;
		const body = document.getElementById("body").value;
		sendRequest(url, method, body);
	});

	// Formulaire d'upload
	document.getElementById("uploadForm").addEventListener("submit", async (e) =>
	{
		e.preventDefault();
		const fileInput = document.getElementById("fileInput");
		const formData = new FormData();

		for (const file of fileInput.files)
			formData.append("files[]", file);

		const response = await fetch("/upload", {
			method: "POST",
			body: formData
		});

		const text = await response.text();
		result.innerText = `Status: ${response.status} ${response.statusText}\n\n${text}`;
	});

	// Formulaire POST CGI (création de fichier)
	const postForm = document.getElementById("postCgiForm");
	if (postForm)
	{
		postForm.addEventListener("submit", async (e) =>
		{
			e.preventDefault();
			const filename = document.getElementById("filename-post").value;
			const res = await fetch("/cgi_methods/post-handler.cgi", {
				method: "POST",
				headers: { "Content-Type": "application/x-www-form-urlencoded" },
				body: `filename=${encodeURIComponent(filename)}`
			});
			const text = await res.text();
			document.getElementById("cgiPostDeleteResult").textContent = text;
		});
	}

	// Formulaire DELETE CGI (suppression de fichier)
	const deleteForm = document.getElementById("deleteCgiForm");
	if (deleteForm)
	{
		deleteForm.addEventListener("submit", async (e) =>
		{
			e.preventDefault();
			const filename = document.getElementById("filename-delete").value;
			const res = await fetch("/cgi_methods/delete-handler.cgi", {
				method: "DELETE",
				headers: { "Content-Type": "application/x-www-form-urlencoded" },
				body: `filename=${encodeURIComponent(filename)}`
			});
			const text = await res.text();
			document.getElementById("cgiPostDeleteResult").textContent = text;
		});
	}

	// Fonction d'envoi de requête HTTP
	window.sendRequest = async function (url, method = 'GET', body = null)
	{
		const response = await fetch(url, {
			method: method,
			headers: { 'Content-Type': 'text/plain' },
			body: method === 'POST' ? body : null
		});

		const text = await response.text();
		result.innerText = `Status: ${response.status} ${response.statusText}\n\n${text}`;
		log.innerText = JSON.stringify({
			url: url,
			method: method,
			status: response.status,
			headers: Object.fromEntries(response.headers.entries())
		}, null, 2);
	};

	// Affichage des cookies
	window.showCookies = function ()
	{
		document.getElementById('cookies').innerText = document.cookie || 'Aucun cookie';
	};
});
