document.addEventListener("DOMContentLoaded", () =>
{
	const form = document.getElementById("signupForm");
	form.addEventListener("submit", async function (e)
	{
		e.preventDefault();

		const username = document.getElementById("username").value;
		const email = document.getElementById("email").value;
		const password = document.getElementById("password").value;
		const confirmPassword = document.getElementById("confirmPassword").value;
		const error = document.getElementById("error");

		if (password !== confirmPassword)
		{
			error.textContent = "Les mots de passe ne correspondent pas.";
			error.style.display = "block";
			return;
		}
		if (password.length < 6)
		{
			error.textContent = "Le mot de passe doit contenir au moins 6 caractères.";
			error.style.display = "block";
			return;
		}
		error.style.display = "none";

		const sessionID = getCookieValue("sessionID");
		if (!sessionID)
		{
			error.textContent = "Session non trouvée.";
			error.style.display = "block";
			return;
		}

		const formData = new URLSearchParams();
		formData.append("sessionID", sessionID);
		formData.append("username", username);
		formData.append("email", email);
		formData.append("password", password);

		const response = await fetch("/cgi_tester/signup.cgi",
		{
			method: "POST",
			headers: { "Content-Type": "application/x-www-form-urlencoded" },
			body: formData.toString(),
		});

		const text = await response.text();
		if (response.ok)
		{
			alert("Compte créé avec succès !");
			window.location.href = "index.html";
		}
		else
		{
			error.textContent = "Erreur lors de la création du compte : " + text;
			error.style.display = "block";
		}
	});
});
