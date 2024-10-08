from flask_wtf import FlaskForm
from wtforms import StringField, PasswordField, IntegerField, TextAreaField, SubmitField
from wtforms.validators import DataRequired, Length, InputRequired, NumberRange
from app import models
from flask import request

class LoginForm(FlaskForm):
    username = StringField('Username', validators=[DataRequired()])
    password = PasswordField('Password', validators=[DataRequired(), Length(min=8)])
    submit_login = SubmitField('Log in')
    submit_signup = SubmitField('Sign Up')

class ReviewCreateForm(FlaskForm):
    star_rating = IntegerField(
        'Star Rating (0-5 stars)',
        validators=[
            InputRequired(message='Star rating is required'),
            NumberRange(min=0, max=5, message='Star rating must be between 0 and 5')
        ]
    )
    review_content = TextAreaField('Review Content', validators=[InputRequired(message='Review content is required')])
    submit = SubmitField('Submit Review')
