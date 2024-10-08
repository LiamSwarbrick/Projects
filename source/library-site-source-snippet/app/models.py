from flask_login import UserMixin
from app import db, login_manager
from sqlalchemy import CheckConstraint
from werkzeug.security import generate_password_hash, check_password_hash
from math import ceil

class User(db.Model, UserMixin):
    id             = db.Column(db.Integer, primary_key=True)
    username       = db.Column(db.String(64), unique=True, nullable=False)
    password_hash  = db.Column(db.String(128), unique=True, nullable=False)
    reviews = db.relationship('Review', backref='user', lazy=True)

    def set_password(self, password):
        self.password_hash = generate_password_hash(password)

    def check_password(self, password):
        return check_password_hash(self.password_hash, password)


class Book(db.Model):
    id                = db.Column(db.Integer, primary_key=True)
    title             = db.Column(db.String(128), nullable=False)
    description       = db.Column(db.Text, nullable=False)
    cover_url         = db.Column(db.String(256), nullable=False)
    reviews           = db.relationship('Review', backref='book', lazy=True)

    def get_average_stars(self):
        if not self.reviews:
            return None  # Return None if there are no reviews
        
        total_stars = sum(review.star_rating for review in self.reviews)
        average_stars = total_stars / len(self.reviews)
        return int(round(average_stars))


class Review(db.Model):
    id           = db.Column(db.Integer, primary_key=True)
    star_rating  = db.Column(db.Integer, CheckConstraint('star_rating >= 0 AND star_rating <= 5'), nullable=False)
    content      = db.Column(db.Text, nullable=False)
    likes        = db.Column(db.Integer, nullable=False, default=0)
    user_id      = db.Column(db.Integer, db.ForeignKey('user.id'), nullable=False)
    book_id      = db.Column(db.Integer, db.ForeignKey('book.id'), nullable=False)


@login_manager.user_loader
def load_user(user_id):
    return User.query.get(int(user_id))


