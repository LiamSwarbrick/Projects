from flask import Flask
from flask_bootstrap import Bootstrap
from flask_sqlalchemy import SQLAlchemy
from flask_admin import Admin
from flask_babel import Babel
from flask_migrate import Migrate
from flask_login import LoginManager

app = Flask(__name__)
app.config.from_object('config')
bootstrap = Bootstrap(app)  # Only incase flask admin depends on it
db = SQLAlchemy(app)
babel = Babel(app)
admin = Admin(app, template_mode='bootstrap4')
login_manager = LoginManager(app)
login_manager.login_view = 'login'

# Only for debug mode since this recreates all table sql
# each time the server is ran
with app.app_context():
    db.create_all()

migrate = Migrate(app, db)

from app import views, models
