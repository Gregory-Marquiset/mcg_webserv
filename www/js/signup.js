document.addEventListener( "DOMContentLoaded", () =>
{
	const form = document.getElementById( "signupForm" );
	const password = document.getElementById( "password" );
	const confirmPassword = document.getElementById( "confirmPassword" );
	const error = document.getElementById( "error" );

	form.addEventListener( "submit", function( e )
	{
		if ( password.value !== confirmPassword.value )
		{
			e.preventDefault();
			error.textContent = "Les mots de passe ne correspondent pas.";
			error.style.display = "block";
			return ;
		}

		if ( password.value.length < 6 )
		{
			e.preventDefault();
			error.textContent = "Le mot de passe doit contenir au moins 6 caractères.";
			error.style.display = "block";
			return ;
		}

		error.style.display = "none";
	});
});
