from multiprocessing import cpu_count

bind = "localhost:50015"
wsgi_app = "main:app"
proc_name = "dac-api"
workers = cpu_count()
worker_class = "uvicorn.workers.UvicornWorker"
backlog = 128
