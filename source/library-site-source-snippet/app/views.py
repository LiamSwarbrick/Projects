# TODO: Only import used functions:
from flask import render_template, flash, redirect, url_for, request, jsonify
from app import app, db, models, admin
from .forms import LoginForm, ReviewCreateForm
from flask_login import login_user, logout_user, current_user, login_required
from flask_admin.contrib.sqla import ModelView
import os
import random

# Flask-Admin for models
admin.add_view(ModelView(models.User, db.session))
admin.add_view(ModelView(models.Book, db.session))
admin.add_view(ModelView(models.Review, db.session))

"""
Function below loads data for the prototype, since there are no users
but we still need to fill the database with books and reviews
To regenerate these open up a python terminal in the root and do:
>>> from app import db, models
>>> models.Book.query.delete()
>>> db.session.delete()
And then run the server again and it will generate the data again.
"""
@app.before_first_request
def on_before_first_request():
    # Init books if empty:
    if not models.Book.query.first():
        # Load books:
        books_folder = 'books/'

        # Make sure book table is empty before loading books
        models.Book.query.delete()

        for filename in os.listdir(books_folder):
            if filename.endswith('.txt'):
                file_path = os.path.join(books_folder, filename)

                with open(file_path, 'r') as file:
                    lines = file.readlines()

                    # Extract data from the file
                    title = lines[0].strip()
                    cover_url = lines[1].strip()
                    description = ''.join(lines[2:]).strip()

                    # Create a new Book instance
                    book = models.Book(title=title, cover_url=cover_url, description=description)

                    # Add the book to the database
                    db.session.add(book)

        # Commit changes to the database
        db.session.commit()
    
        # Add reviews add some for the sake of the prototype:
        # Create some users first that will write reviews for us:
        try:
            # Make sure users table is empty
            models.User.query.delete()
            models.Review.query.delete()
            db.session.commit()
        except Exception as e:
            # Rollback in case of an error
            db.session.rollback()
            print(f'Error emptying users table: {str(e)}')

        users = []
        testusernames = [  # Generated with https://www.fantasynamegenerators.com/username-generator.php
            "Sitcommander",
            "Starfissure",
            "Salmonster",
            "Dolphine",
            "AcrobaticOracle",
            "GloriousZebra",
            "FruitStitches",
            "ShallowTauren",
            "RainLizard",
            "MimicCoconut",
            "Termime",
            "Sniperipheral",
            "Lemurmur",
            "Sha-Doh"
            "OldWarrior",
            "ActionMaverick",
            "ForestJudge",
            "MeanSaint",
            "PoisonTwin",
            "DazzlingMosquito"]
        
        for testusername in testusernames:
            user = models.User(username=testusername)
            user.set_password("password")
            users.append(user)

            db.session.add(book)
        
        db.session.commit()
        
        review_templates = [
        """This book kinda sucks not gonna lie, its boring don't read it.
        OMG IT MAKES ME ANGRY HOW BAD IT IS.
        I HOPE THIS GETS REMOVED OFF THE SHELVES.
        >:(""",
        """Felt like I wasted my time with this one.
        It's not really worth powering through as nothing interesting ever happens
        Try some other classics, you'll get a lot more enjoyment!
        :(""",
        """Quite an interesting read this one.
        I wish it good to the good stuff sooner though :/
        I almost dropped it but then it suddenly captivated me quick a bit.
        A decent read for a train journey.
        :|""",
        """If your big on reading the classics, you'll definitely find yourself reading this eventually.
        Took me a while to get round to it, but I'm glad I did read it, I found it quick the interesting read!
        :P""",
        """This book has so many good ideas packed into it.
        You're in for a great read from the start, I'm sure you won't put it down.
        :)""",
        """Honestly one of my favourite books, can't believe I hadn't read it sooner.
        I got all my students to read this book and their I've seen all their writing styles improve dramatically!
        Firstly, this book is quite important in many circles, you can't not know about this book if you want to call yourself an academic!
        And secondly, this book is so captivating you'll definitely get through it pretty quick,
        so no reason not to pick it up, its not much of an investment!!!
        :O <3"""]

        # Lean towards higher ratings:
        star_rating_choices = [0,0,1,2,2,3,3,3,4,4,5,5,5,5,5,5]
        star_rating_choices_moby_dick = [0,1,2,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5]

        for book in models.Book.query.all():
            for user in users:
                # Randomly select a star rating
                star_rating = random.choice(star_rating_choices)
                if book.id == 1:
                    star_rating = random.choice(star_rating_choices_moby_dick)
                review_content = review_templates[star_rating]

                # Create a new review for the user and book
                new_review = models.Review(
                    star_rating=star_rating,
                    content=review_content,
                    user=user,
                    book=book
                )

                # Add the review to the session
                db.session.add(new_review)
        
        db.session.commit()  


###########################################################################
#####                        WEBSITE PAGES                            #####
###########################################################################

@app.route('/')
def entrypoint():
    return redirect(url_for('library'))

@app.route('/login', methods=['GET', 'POST'])
def login():
    if current_user.is_authenticated:
        return redirect(url_for('dashboard'))

    form = LoginForm()

    if form.validate_on_submit():
        username = form.username.data
        password = form.password.data

        if form.submit_login.data:
            user = models.User.query.filter_by(username=username).first()

            if user and user.check_password(password):
                login_user(user)
                flash('Login successful!', 'success')
                return redirect(url_for('dashboard'))

            else:
                flash('Incorrect username or password. Please try again.', 'danger')
        elif form.submit_signup.data:
            if models.User.query.filter_by(username=username).first():
                flash('Username already in use. Please try another one.', 'danger')
            else:
                new_user = models.User(username=username)
                new_user.set_password(password)
                db.session.add(new_user)
                db.session.commit()

                flash('Account created successfully. Logging In.', 'success')
                login_user(new_user)
                return redirect(url_for('dashboard'))

    return render_template('login.html', form=form)


@app.route('/logout')
@login_required
def logout():
    logout_user()
    return redirect(url_for('entrypoint'))

@app.route('/dashboard')
@login_required
def dashboard():
    user_reviews = models.Review.query.filter_by(user_id=current_user.id).all()

    return render_template('dashboard.html', user_reviews=user_reviews)

@app.route('/library')
# @login_required
def library():
    books = models.Book.query.all()
    return render_template('library.html', books=books)

@app.route('/load-more-books', methods=['GET'])
def load_more_books():
    page = request.args.get('page', 1, type=int)
    per_page = 6
    books = models.Book.query.paginate(page, per_page, error_out=False)

    if not books.items:
        return jsonify({'books': []})
    
    rendered_books = render_template('partials/_book_partial.html', books=books.items)
    return jsonify({'books': rendered_books})

@app.route('/library/<int:id>', methods=['GET', 'POST'])
@login_required
def book_page(id):
    book = models.Book.query.get(id)
    form = ReviewCreateForm()
    user_review = models.Review.query.filter_by(book=book, user=current_user).first()

    if form.validate_on_submit():
        star_rating = form.star_rating.data
        content = form.review_content.data

        new_review = models.Review(star_rating=star_rating, content=content, user_id=current_user.id, book_id=id)
        if not user_review:
            db.session.add(new_review)
            flash('Review submitted successfully', 'success')
        else:
            user_review.star_rating = star_rating
            user_review.content = content
            flash('Review updated successfully!', 'success')

        db.session.commit()

        # Return a JSON response for AJAX requests
        if request.headers.get('X-Requested-With') == 'XMLHttpRequest':
            return jsonify({'status': 'success', 'message': 'Review submitted successfully'})

        # Redirect to the book page after form submission
        return redirect(url_for('book_page', id=id))

    if book:
        book_code = book.cover_url.split('/')[-1].split('-')[0]
        large_cover_url = f"https://covers.openlibrary.org/b/id/{book_code}-L.jpg"

        if request.headers.get('X-Requested-With') == 'XMLHttpRequest':
            return jsonify({'status': 'error', 'errors': form.errors})

        return render_template('book_page.html', book=book, large_cover_url=large_cover_url, form=form, user_review=user_review)
    else:
        return render_template('book_not_found.html')

@app.route('/delete_review/<int:review_id>', methods=['POST'])
@login_required
def delete_review(review_id):
    review = models.Review.query.get_or_404(review_id)

    # Check if the current user is the owner of the review
    if current_user != review.user:
        flash("You don't have permission to delete this review.", 'danger')
        return redirect(url_for('book_page', id=review.book_id))

    # Get the book ID before deleting the review
    book_id = review.book_id

    # Perform the deletion
    db.session.delete(review)
    db.session.commit()

    flash('Your review has been deleted.', 'success')
    return redirect(url_for('book_page', id=book_id))
